// Fill out your copyright notice in the Description page of Project Settings.


#include "FluidArt/MetaFluidArtActorComponent.h"

#include "NiagaraSystem.h"
#include "Components/SplineComponent.h"
#include "Engine/StaticMeshActor.h"
#include "FluidArt/MetaFluidArtSaveGame.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values for this component's properties
UMetaFluidArtActorComponent::UMetaFluidArtActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them. 
	PrimaryComponentTick.bCanEverTick = false;
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemFinder(TEXT("/Script/Niagara.NiagaraSystem'/MetaVFX/FluidArt/VFX/NS_Fluid.NS_Fluid'"));
	if (NiagaraSystemFinder.Succeeded())
	{
		NiagaraSystem = NiagaraSystemFinder.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (SphereMeshFinder.Succeeded())
	{
		StaticMesh = SphereMeshFinder.Object;
	}
}

void UMetaFluidArtActorComponent::PostInitProperties()
{
	Super::PostInitProperties();

	SetAsset(NiagaraSystem);
}

// Called when the game starts
void UMetaFluidArtActorComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->Tags.Add(TEXT("FluidArtActor"));

	const FNiagaraData NiagaraData = UMetaFluidArtSaveGame::Get()->NiagaraData;
	
	NumCellsMaxAxis = NiagaraData.NumCellsMaxAxis;
	ParticlePerCell = NiagaraData.ParticlePerCell;
	PressureIterations = NiagaraData.PressureIterations;
	TextureChangeDelay = NiagaraData.TextureChangeDelay;
	SpriteScale = NiagaraData.SpriteScale;
	UVStep = NiagaraData.UVStep;
	FilePaths = NiagaraData.FilePaths;

	NoiseForce = NiagaraData.NoiseForce;
	NoiseFrequency = NiagaraData.NoiseFrequency;
	bAutoNoise = NiagaraData.bAutoNoise;
	TotalTime = 10 / NoiseForce;

	for (const FString FilePath : FilePaths)
	{
		if (UTexture2D* LoadedTexture = UKismetRenderingLibrary::ImportFileAsTexture2D(GetWorld(), FilePath))
		{
			Textures.Emplace(LoadedTexture);
		}
	}

	SetVariableInt(TEXT("Num Cells Max Axis"), NumCellsMaxAxis);
	SetVariableInt(TEXT("Particles Per Cell"), ParticlePerCell);
	SetVariableInt(TEXT("Pressure Iterations"), PressureIterations);
	SetVariableFloat(TEXT("TextureChangeDelay"), TextureChangeDelay);
	SetVariableFloat(TEXT("SpriteScale"), SpriteScale);
	SetVariableVec2(TEXT("UVStep"), UVStep);

	// Setup noise
	UpdateNoiseCollider();
	UpdateNoiseMovement();
	
	GetWorld()->GetTimerManager().SetTimer(TextureChangeHandle, this, &UMetaFluidArtActorComponent::NextTexture, TextureChangeDelay - 0.1f, true);

	NextTexture();
}

void UMetaFluidArtActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (TextureChangeHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TextureChangeHandle);
		TextureChangeHandle.Invalidate();
	}

	if (NoiseHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(NoiseHandle);
		NoiseHandle.Invalidate();
	}
}

void UMetaFluidArtActorComponent::NextTexture()
{
	if (Textures.IsEmpty()) return;
	
	if (++CurrentIndex >= Textures.Num()) CurrentIndex = 0;
	SetVariableTexture(TEXT("Texture"), Textures[CurrentIndex]);

	if (bAutoNoise)
	{
		UpdateNoiseCollider();

		ElapsedTime = 0;
		if (NoiseHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(NoiseHandle);
			NoiseHandle.Invalidate();
		}
		UpdateNoiseMovement();
	}
		
}

void UMetaFluidArtActorComponent::UpdateNoiseMovement()
{
	if (NoiseForce == 0) return;
	
	TotalTime = FMath::Clamp(Duration, 0, TextureChangeDelay * 0.6f);
	if (ElapsedTime < TotalTime)
	{
		ElapsedTime += 0.01f;
		float Alpha = (ElapsedTime / TotalTime) * NoiseForce;
		Alpha -= floor(Alpha);

		for (auto NoiseCollider : NoiseColliders)
		{
			if (USplineComponent* SplineComponent = Cast<USplineComponent>(NoiseCollider->GetComponentByClass(USplineComponent::StaticClass())))
			{
				// 현재 Spline의 전체 길이를 구함
				const float SplineLength = SplineComponent->GetSplineLength();
				// 알파 값을 이용해 현재 위치까지의 거리를 계산
				float CurrentDistance = SplineLength * Alpha;
				// 현재 거리를 기준으로 월드 좌표계상의 위치를 계산 (회전, 스케일 등의 정보도 함께 받을 수 있음)
				FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local);
				// 액터의 위치 업데이트
				NoiseCollider->SetActorLocation(NewLocation);	
			}
		}

		GetWorld()->GetTimerManager().SetTimer(NoiseHandle, this, &UMetaFluidArtActorComponent::UpdateNoiseMovement, 0.01f, false);
	}
	else
	{
		ElapsedTime = 0;
		for (auto& NoiseCollider : NoiseColliders)
		{
			NoiseCollider->Destroy();
		}
		NoiseColliders.Empty();
	}
}

void UMetaFluidArtActorComponent::UpdateSpline(USplineComponent* SplineComponent)
{
	SplineComponent->ClearSplinePoints();
	SplineComponent->SetClosedLoop(true);

	auto GenerateRandomPoint = [this]() -> FVector {
		return GetComponentLocation() + StartLocationOffset + FVector(
			FMath::FRandRange(-NoiseRange.X * 0.5f, NoiseRange.X * 0.5f),
			FMath::FRandRange(-NoiseRange.Y * 0.5f, NoiseRange.Y * 0.5f),
			FMath::FRandRange(0.0f, NoiseRange.Z));
	};

	const FVector BaseLocation = GetComponentLocation() + StartLocationOffset;
	TArray<FVector> Vertexs;
	
	Vertexs.Emplace(BaseLocation + FVector( NoiseRange.X * 0.5f,  NoiseRange.Y * 0.5f, NoiseRange.Z ));
	Vertexs.Emplace(BaseLocation + FVector(-NoiseRange.X * 0.5f,  NoiseRange.Y * 0.5f, NoiseRange.Z ));
	Vertexs.Emplace(BaseLocation + FVector(-NoiseRange.X * 0.5f, -NoiseRange.Y * 0.5f, NoiseRange.Z ));
	Vertexs.Emplace(BaseLocation + FVector( NoiseRange.X * 0.5f, -NoiseRange.Y * 0.5f, NoiseRange.Z ));
	Vertexs.Emplace(BaseLocation + FVector( NoiseRange.X * 0.5f,  NoiseRange.Y * 0.5f,  0.0f ));
	Vertexs.Emplace(BaseLocation + FVector(-NoiseRange.X * 0.5f,  NoiseRange.Y * 0.5f,  0.0f ));
	Vertexs.Emplace(BaseLocation + FVector(-NoiseRange.X * 0.5f, -NoiseRange.Y * 0.5f,  0.0f ));
	Vertexs.Emplace(BaseLocation + FVector( NoiseRange.X * 0.5f, -NoiseRange.Y * 0.5f,  0.0f ));
	
	
	FVector PrevVector = FVector::Zero();
	for (int32 j = 0; j < NoiseFrequency; ++j)
	{
		FVector RandomVector = GenerateRandomPoint();

		if (j == 0) PrevVector = RandomVector;
		float MaxDist = 0;
		float MinDist = 999999;
		for (auto Vector : Vertexs)
		{
			const float EdgeDist = FVector::Dist(PrevVector, Vector);
			if (MaxDist < EdgeDist)
			{
				MaxDist = EdgeDist;
			}
			if (MinDist > EdgeDist)
			{
				MinDist = EdgeDist;
			}
		}

		if (MaxDist > MaxLength) MaxDist = MaxLength;
		if (MinDist > MinLength) MinDist = MinLength;
		if (MinDist + 100 > MaxDist) MinDist = MaxDist - 100;
		
		while (FVector::Dist(PrevVector, RandomVector) < MinDist || FVector::Dist(PrevVector, RandomVector) > MaxDist)
		{
			RandomVector = GenerateRandomPoint();
		}
		PrevVector = RandomVector;
        
		SplineComponent->AddSplinePoint(RandomVector, ESplineCoordinateSpace::World);
	}
}

void UMetaFluidArtActorComponent::UpdateNoiseCollider()
{
	if (NoiseForce == 0) return;

	for (auto& NoiseCollider : NoiseColliders)
	{
		NoiseCollider->Destroy();
	}
	NoiseColliders.Empty();
	
	for (int i = 0; i < ColliderNum; ++i)
	{
		AStaticMeshActor* NoiseCollider = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
		NoiseCollider->Tags.Emplace(TEXT("Collider"));
		NoiseCollider->SetMobility(EComponentMobility::Type::Movable);
		NoiseCollider->SetActorScale3D(NoiseColliderScale);
		NoiseCollider->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
		NoiseCollider->GetStaticMeshComponent()->SetVisibility(bColliderVisible);
		NoiseColliders.Emplace(NoiseCollider);

		USplineComponent* SplineComponent = Cast<USplineComponent>(NoiseCollider->AddComponentByClass(USplineComponent::StaticClass(), false, FTransform(), true));
		UpdateSpline(SplineComponent);
	}
}