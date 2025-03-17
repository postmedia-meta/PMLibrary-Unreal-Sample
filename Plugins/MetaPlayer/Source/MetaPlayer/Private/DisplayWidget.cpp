// Fill out your copyright notice in the Description page of Project Settings.


#include "DisplayWidget.h"

#include "MediaTexture.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"

UDisplayWidget::UDisplayWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Script/Engine.Material'/MetaPlayer/Demo/M_Display_UI.M_Display_UI'"));
	if (MaterialFinder.Succeeded())
	{
		DisplayMaterial = MaterialFinder.Object;
	}
}

void UDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DisplayMaterial == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("DisplayMaterial is nullptr!!!"));
		return;
	}

	for (UWidget* Display : DisplayPanel->GetAllChildren())
	{
		if (UImage* DisplayWidget = Cast<UImage>(Display))
		{
			UMaterialInstanceDynamic* MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DisplayMaterial, nullptr);
			MaterialInstanceDynamics.Emplace(MaterialInstanceDynamic);
			DisplayWidget->SetBrushFromMaterial(MaterialInstanceDynamic);

			Displays.Emplace(DisplayWidget);
		}
	}
}

void UDisplayWidget::SetMediaTexturesDisplayMaterials(const TArray<UMediaTexture*>& MediaTextures)
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
