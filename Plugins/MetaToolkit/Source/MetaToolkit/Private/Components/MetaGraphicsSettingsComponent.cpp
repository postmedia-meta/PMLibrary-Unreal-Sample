

#include "Components//MetaGraphicsSettingsComponent.h"

#include "Widgets/MTKGraphicsSettingsWidget.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"

UMetaGraphicsSettingsComponent::UMetaGraphicsSettingsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UUserWidget> GraphicsSettingWidgetClassFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/MetaToolkit/Setting/UIs/WBP_MTKGraphicsSettingWidget.WBP_MTKGraphicsSettingWidget_C'"));
	if (GraphicsSettingWidgetClassFinder.Succeeded())
	{
		GraphicsSettingWidgetClass = GraphicsSettingWidgetClassFinder.Class;
	}
}

void UMetaGraphicsSettingsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("This component can only be used as a component of a player controller."));
		return;
	}

	if (GraphicsSettingWidgetClass)
	{
		GraphicsSettingsWidget = CreateWidget<UMTKGraphicsSettingsWidget>(PlayerController, GraphicsSettingWidgetClass);
		if (GraphicsSettingsWidget)
		{
			GraphicsSettingsWidget->AddToViewport(ZOrder);
			GraphicsSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UMetaGraphicsSettingsComponent::ShowGraphicsSettingsUI()
{
	if (GraphicsSettingsWidget)
	{
		GraphicsSettingsWidget->ShowWidget();
	}
}

void UMetaGraphicsSettingsComponent::HideGraphicsSettingsUI()
{
	if (GraphicsSettingsWidget)
	{
		GraphicsSettingsWidget->HideWidget();
	}
}

bool UMetaGraphicsSettingsComponent::IsShowWidget()
{
	if (GraphicsSettingsWidget)
	{
		return GraphicsSettingsWidget->GetVisibility() != ESlateVisibility::Collapsed;
	}

	return false;
}
