

#include "MetaMagicTrailComponent.h"

#include "MemoryPoolObject.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

UMetaMagicTrailComponent::UMetaMagicTrailComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<ANiagaraActor> MagicTrailNiagaraActorFinder(TEXT("/Script/Engine.Blueprint'/MetaVFX/MagicTrail/VFX/MagicTrail/BP_MagicTrailNiagaraActor.BP_MagicTrailNiagaraActor_C'"));
	if (MagicTrailNiagaraActorFinder.Succeeded())
	{
		MagicTrailNiagaraActorClass = MagicTrailNiagaraActorFinder.Class;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> SpriteMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Sprite.M_Sprite'"));
	if (SpriteMaterialFinder.Succeeded())
	{
		SpriteMaterial = SpriteMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> TrailMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Trail.M_Trail'"));
	if (TrailMaterialFinder.Succeeded())
	{
		TrailMaterial = TrailMaterialFinder.Object;
	}
}

void UMetaMagicTrailComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("This component can only be used as a component of a player controller."));
		return;
	}
	SpriteMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(SpriteMaterial, nullptr);
	TrailMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(TrailMaterial, nullptr);
	SpriteMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), Texture2D);
	TrailMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), Texture2D);
	
	MemoryPoolObject = NewObject<UMemoryPoolObject>(PlayerController, TEXT("Memory Pool"));
	MemoryPoolObject->SetActorClass(MagicTrailNiagaraActorClass);
	MemoryPoolObject->CreateActors(10);

	if (bUseMouse)
	{
		// 액터의 InputComponent를 가져옵니다.
		if (UInputComponent* InputComp = PlayerController->FindComponentByClass<UInputComponent>())
		{
			InputComp->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &UMetaMagicTrailComponent::OnLeftMouseDown);
			InputComp->BindKey(EKeys::LeftMouseButton, IE_Released, this, &UMetaMagicTrailComponent::OnLeftMouseUp);
		}
	}

	FViewport::ViewportResizedEvent.AddUObject(this, &UMetaMagicTrailComponent::OnViewportResized);

	if (ViewportSize.X <= 0|| ViewportSize.Y <= 0)
	{
		PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);
	}
}

void UMetaMagicTrailComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (MouseDelayHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
		MouseDelayHandle.Invalidate();
	}

	FViewport::ViewportResizedEvent.Clear();
}

void UMetaMagicTrailComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentTime += DeltaTime;
	if (CurrentTime > ResetTime)
	{
		CurrentTime = 0;
		DeallocateAllMagicTrails();
	}
	
	if (bIsDragging) LeftMouseDrag();
}

void UMetaMagicTrailComponent::NewMagicTrailsWithLiDAR(const TArray<int32> IDs, const TArray<FVector2D> Percentages)
{
	if (IDs.Num() != Percentages.Num()) return;
	
	for (int32 i = 0; i < IDs.Num(); ++i)
	{
		bool IsExist = false;
		for (int32 j = 0; j < LiDARActors.Num(); ++j)
		{
			if (LiDARActors[j].ID == IDs[i])
			{
				IsExist = true;
				break;
			}
		}
		
		if (!IsExist)
		{
			FLiDARActor LiDARActor;
			LiDARActor.ID = IDs[i];
			LiDARActor.Actor = MemoryPoolObject->AllocateActor().Actor;
			LiDARActor.NiagaraComponent = Cast<ANiagaraActor>(LiDARActor.Actor)->GetNiagaraComponent();
			LiDARActor.NiagaraComponent->SetVariableMaterial(TEXT("MI_Sprite"), SpriteMaterialInstanceDynamic);
			LiDARActor.NiagaraComponent->SetVariableMaterial(TEXT("MI_Trail"), TrailMaterialInstanceDynamic);
			LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("LifeTime"), LifeTime);
			LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("Scale"), Scale);
			LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("EdgeThickness"), EdgeThickness);
			LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("EdgeIntensity"), EdgeIntensity);
			LiDARActor.NiagaraComponent->SetVariableLinearColor(TEXT("EdgeColor"), EdgeColor);
			LiDARActor.NiagaraComponent->SetVariableLinearColor(TEXT("SpriteColor"), SpriteColor);
			LiDARActor.NiagaraComponent->SetVariableBool(TEXT("bAutoColor"), bAutoColor);

			LiDARActors.Emplace(LiDARActor);
			SetActorLocationToScreenPosition(LiDARActor, Percentages[i].X, Percentages[i].Y);	
		}
	}
}

void UMetaMagicTrailComponent::UpdateMagicTrailsWithLiDAR(const TArray<int32> IDs, const TArray<FVector2D> Percentages)
{
	if (IDs.Num() != Percentages.Num()) return;
	
	for (int32 i = 0; i < IDs.Num(); ++i)
	{
		bool IsExist = false;
		for (int32 j = 0; j < LiDARActors.Num(); ++j)
		{
			if (LiDARActors[j].ID == IDs[i])
			{
				IsExist = true;
				SetActorLocationToScreenPosition(LiDARActors[j], Percentages[i].X, Percentages[i].Y);	
				break;
			}
		}
		
		if (!IsExist)
		{
			FLiDARActor LiDARActor;
			LiDARActor.ID = IDs[i];
			LiDARActor.Actor = MemoryPoolObject->AllocateActor().Actor;
			LiDARActor.NiagaraComponent = Cast<ANiagaraActor>(LiDARActor.Actor)->GetNiagaraComponent();
			LiDARActor.NiagaraComponent->SetVariableMaterial(TEXT("MI_Sprite"), SpriteMaterialInstanceDynamic);
			LiDARActor.NiagaraComponent->SetVariableMaterial(TEXT("MI_Trail"), TrailMaterialInstanceDynamic);
			LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("LifeTime"), LifeTime);
			LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("Scale"), Scale);
			LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("EdgeThickness"), EdgeThickness);
			LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("EdgeIntensity"), EdgeIntensity);
			LiDARActor.NiagaraComponent->SetVariableLinearColor(TEXT("EdgeColor"), EdgeColor);
			LiDARActor.NiagaraComponent->SetVariableLinearColor(TEXT("SpriteColor"), SpriteColor);
			LiDARActor.NiagaraComponent->SetVariableBool(TEXT("bAutoColor"), bAutoColor);

			LiDARActors.Emplace(LiDARActor);
			SetActorLocationToScreenPosition(LiDARActor, Percentages[i].X, Percentages[i].Y);	
		}
	}
}

void UMetaMagicTrailComponent::RemoveMagicTrailsWithLiDAR(const TArray<int32> IDs)
{
	TArray<int32> RemoveItems;
	RemoveItems.Empty();
	
	for (int32 i = 0; i < IDs.Num(); ++i)
	{
		for (int32 j = 0; j < LiDARActors.Num(); ++j)
		{
			if (LiDARActors[j].ID == IDs[i])
			{
				RemoveItems.Emplace(j);
				
				FLiDARActor& LiDARActor = LiDARActors[j];
				if (LiDARActor.DelayHandle.IsValid())
				{
					GetWorld()->GetTimerManager().ClearTimer(LiDARActor.DelayHandle);
					LiDARActor.DelayHandle.Invalidate();
				}
		
				LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
				MemoryPoolObject->DeallocateActor(LiDARActor.Actor, false);

				LiDARActor.Actor = nullptr;
				LiDARActor.NiagaraComponent = nullptr;
			}
		}
	}

	for (const int32 RemoveItem : RemoveItems)
	{
		LiDARActors.RemoveAt(RemoveItem);
	}
}

void UMetaMagicTrailComponent::DeallocateAllMagicTrails()
{
	if (MouseActor)
	{
		if (MouseDelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
			MouseDelayHandle.Invalidate();
		}
		
		MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(MouseActor, false);

		MouseActor = nullptr;
		MouseNiagaraComponent = nullptr;
	}

	for (FLiDARActor& LiDARActor : LiDARActors)
	{
		if (LiDARActor.DelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(LiDARActor.DelayHandle);
			LiDARActor.DelayHandle.Invalidate();
		}
		
		LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(LiDARActor.Actor, false);

		LiDARActor.Actor = nullptr;
		LiDARActor.NiagaraComponent = nullptr;
	}
	
	LiDARActors.Empty();
}

void UMetaMagicTrailComponent::OnViewportResized(FViewport* Viewport, unsigned int I)
{
	ViewportSize = Viewport->GetSizeXY();
}

void UMetaMagicTrailComponent::OnLeftMouseDown()
{
	bIsDragging = true;
	if (MouseActor == nullptr)
	{
		MouseActor = MemoryPoolObject->AllocateActor().Actor;
		MouseNiagaraComponent = Cast<ANiagaraActor>(MouseActor)->GetNiagaraComponent();
		MouseNiagaraComponent->SetVariableMaterial(TEXT("MI_Sprite"), SpriteMaterialInstanceDynamic);
		MouseNiagaraComponent->SetVariableMaterial(TEXT("MI_Trail"), TrailMaterialInstanceDynamic);
		MouseNiagaraComponent->SetVariableFloat(TEXT("LifeTime"), LifeTime);
		MouseNiagaraComponent->SetVariableFloat(TEXT("Scale"), Scale);
		MouseNiagaraComponent->SetVariableFloat(TEXT("EdgeThickness"), EdgeThickness);
		MouseNiagaraComponent->SetVariableFloat(TEXT("EdgeIntensity"), EdgeIntensity);
		MouseNiagaraComponent->SetVariableLinearColor(TEXT("EdgeColor"), EdgeColor);
		MouseNiagaraComponent->SetVariableLinearColor(TEXT("SpriteColor"), SpriteColor);
		MouseNiagaraComponent->SetVariableBool(TEXT("bAutoColor"), bAutoColor);

		SetMouseActorLocation();
	}
}

void UMetaMagicTrailComponent::OnLeftMouseUp()
{
	bIsDragging = false;
	if (MouseActor)
	{
		if (MouseDelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
			MouseDelayHandle.Invalidate();
		}
		
		MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(MouseActor, false);
	
		MouseActor = nullptr;
		MouseNiagaraComponent = nullptr;
	}
}

void UMetaMagicTrailComponent::LeftMouseDrag()
{
	if (MouseActor == nullptr)
	{
		MouseActor = MemoryPoolObject->AllocateActor().Actor;
		MouseNiagaraComponent = Cast<ANiagaraActor>(MouseActor)->GetNiagaraComponent();
		MouseNiagaraComponent->SetVariableMaterial(TEXT("MI_Sprite"), SpriteMaterialInstanceDynamic);
		MouseNiagaraComponent->SetVariableMaterial(TEXT("MI_Trail"), TrailMaterialInstanceDynamic);
		MouseNiagaraComponent->SetVariableFloat(TEXT("LifeTime"), LifeTime);
		MouseNiagaraComponent->SetVariableFloat(TEXT("Scale"), Scale);
		MouseNiagaraComponent->SetVariableFloat(TEXT("EdgeThickness"), EdgeThickness);
		MouseNiagaraComponent->SetVariableFloat(TEXT("EdgeIntensity"), EdgeIntensity);
		MouseNiagaraComponent->SetVariableLinearColor(TEXT("EdgeColor"), EdgeColor);
		MouseNiagaraComponent->SetVariableLinearColor(TEXT("SpriteColor"), SpriteColor);
		MouseNiagaraComponent->SetVariableBool(TEXT("bAutoColor"), bAutoColor);
	}
	
	SetMouseActorLocation();
}

void UMetaMagicTrailComponent::SetMouseActorLocation()
{
	if (bUseRay)
	{
		FHitResult HitResult;
		if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
		{
			MouseActor->SetActorLocation(HitResult.Location);
			if (!MouseDelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(MouseDelayHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
				}), 0.1, false);		
			}
		}
		else
		{
			if (MouseDelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
				MouseDelayHandle.Invalidate();

				MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
			}
		}
	}
	else
	{
		FVector WorldPos = FVector::Zero();
		FVector WorldDir = FVector::Zero();
		if (PlayerController->DeprojectMousePositionToWorld(WorldPos, WorldDir))
		{
			MouseActor->SetActorLocation(WorldPos);
			if (!MouseDelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(MouseDelayHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
				}), 0.1, false);		
			}
		}
	}
}

void UMetaMagicTrailComponent::SetActorLocationToScreenPosition(FLiDARActor& LiDARActor, const float X, const float Y)
{
	const FVector2D ScreenPosition = FVector2D(ViewportSize.X * X, ViewportSize.Y * Y);
	
	if (bUseRay)
	{
		FHitResult HitResult;
		if (PlayerController->GetHitResultAtScreenPosition(ScreenPosition, ECC_Visibility, false, HitResult))
		{
			LiDARActor.Actor->SetActorLocation(HitResult.Location);
			if (!LiDARActor.DelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(LiDARActor.DelayHandle, FTimerDelegate::CreateLambda([this, LiDARActor]()->void
				{
					LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
				}), 0.1, false);		
			}
		}
		else
		{
			if (LiDARActor.DelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(LiDARActor.DelayHandle);
				LiDARActor.DelayHandle.Invalidate();

				LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
			}
		}
	}
	else
	{
		FVector WorldPos = FVector::Zero();
		FVector WorldDir = FVector::Zero();
		if (UGameplayStatics::DeprojectScreenToWorld(PlayerController, ScreenPosition, WorldPos, WorldDir))
		{
			LiDARActor.Actor->SetActorLocation(WorldPos);
			if (!LiDARActor.DelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(LiDARActor.DelayHandle, FTimerDelegate::CreateLambda([this, LiDARActor]()->void
				{
					LiDARActor.NiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
				}), 0.1, false);		
			}
		}
	}
}