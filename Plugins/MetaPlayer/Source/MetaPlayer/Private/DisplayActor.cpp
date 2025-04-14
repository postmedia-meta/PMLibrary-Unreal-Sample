// Fill out your copyright notice in the Description page of Project Settings.


#include "DisplayActor.h"

#include "MediaTexture.h"
#include "MultiMediaSyncPlayer.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ADisplayActor::ADisplayActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Component"));
	SetRootComponent(ArrowComponent);

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Script/Engine.Material'/MetaPlayer/Demo/M_Display.M_Display'"));
	if (MaterialFinder.Succeeded())
	{
		DisplayMaterial = MaterialFinder.Object;
	}
}

// Called when the game starts or when spawned
void ADisplayActor::BeginPlay()
{
	Super::BeginPlay();

	if (DisplayMaterial == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DisplayMaterial is nullptr!!!"));
		return;
	}

	TArray<USceneComponent*> RootChildren;
	RootComponent->GetChildrenComponents(false, RootChildren);
	for (USceneComponent* Child : RootChildren)
	{
		if (UStaticMeshComponent* DisplayComponent = Cast<UStaticMeshComponent>(Child))
		{
			UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DisplayMaterial, nullptr);
			MaterialInstanceDynamics.Emplace(MaterialInstanceDynamic);
			DisplayComponent->SetMaterial(0, MaterialInstanceDynamic);
			
			Displays.Emplace(DisplayComponent);
		}
	}

	if (MultiMediaSyncPlayer)
	{
		MultiMediaSyncPlayer->OnPlayerStateChangeEvent.AddDynamic(this, &ADisplayActor::OnPlayerStateChange);
	}
}

void ADisplayActor::SetMediaTexturesDisplayMaterials(const TArray<UMediaTexture*>& MediaTextures)
{
	if (MediaTextures.Num() != MaterialInstanceDynamics.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("[MediaTextures num error] MaterialInstanceDynamics.Num() != MediaTextures.Num()"));
		return;
	}
	
	for (int i = 0; i < MediaTextures.Num(); ++i)
	{
		if (MaterialInstanceDynamics.Num() > i)
		{
			MaterialInstanceDynamics[i]->SetTextureParameterValue(TEXT("Texture"), MediaTextures[i]);
		}
	}
}

void ADisplayActor::OnPlayerStateChange(const EMultiMediaState MultiMediaState)
{
	if (MultiMediaState == EMultiMediaState::Ready)
	{
		SetMediaTexturesDisplayMaterials(MultiMediaSyncPlayer->GetMediaTextures());
	}
}
