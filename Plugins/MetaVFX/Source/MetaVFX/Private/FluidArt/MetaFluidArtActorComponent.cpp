// Fill out your copyright notice in the Description page of Project Settings.


#include "FluidArt/MetaFluidArtActorComponent.h"

#include "NiagaraSystem.h"
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
	NoiseForce = NiagaraData.NoiseForce;
	NoiseFrequency = NiagaraData.NoiseFrequency;
	bAutoNoise = NiagaraData.bAutoNoise;
	TextureChangeDelay = NiagaraData.TextureChangeDelay;
	SpriteScale = NiagaraData.SpriteScale;
	UVStep = NiagaraData.UVStep;
	FilePaths = NiagaraData.FilePaths;

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
	SetVariableFloat(TEXT("NoiseForce"), NoiseForce);
	SetVariableFloat(TEXT("NoiseFrequency"), NoiseFrequency);
	SetVariableFloat(TEXT("TextureChangeDelay"), TextureChangeDelay);
	SetVariableFloat(TEXT("SpriteScale"), SpriteScale);
	SetVariableVec2(TEXT("UVStep"), UVStep);

	NextTexture();

	FTimerHandle TextureChangeHandle;
	GetWorld()->GetTimerManager().SetTimer(TextureChangeHandle, this, &UMetaFluidArtActorComponent::NextTexture, TextureChangeDelay - 0.1f, true);
}

void UMetaFluidArtActorComponent::NextTexture()
{
	if (Textures.IsEmpty()) return;
	
	if (++CurrentIndex >= Textures.Num()) CurrentIndex = 0;
	SetVariableTexture(TEXT("Texture"), Textures[CurrentIndex]);

	if (bAutoNoise)
	{
		constexpr float MinimumForce = 10;
		constexpr float MinimumFrequency = 5;
		
		float Force = FMath::RandRange(-NoiseForce, NoiseForce);
		Force = Force == 0 ? MinimumForce : FMath::Sign(Force) * FMath::Max(MinimumForce, FMath::Abs(Force));

		float Frequency = FMath::RandRange(-NoiseFrequency, NoiseFrequency);
		Frequency = Frequency == 0 ? MinimumFrequency : FMath::Sign(Frequency) * FMath::Max(MinimumFrequency, FMath::Abs(Frequency));

		SetVariableFloat(TEXT("NoiseForce"), Force);
		SetVariableFloat(TEXT("NoiseFrequency"), Frequency);
	}
}
