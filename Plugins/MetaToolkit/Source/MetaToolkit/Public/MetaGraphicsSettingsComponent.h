#pragma once

#include "CoreMinimal.h"
#include "MetaGraphicsSettingsComponent.generated.h"

class UMetaToolkitSaveGame;
class UMetaGraphicsSettingsWidget;

UCLASS(ClassGroup = "MetaToolkit", meta = (BlueprintSpawnableComponent))
class METATOOLKIT_API UMetaGraphicsSettingsComponent : public UActorComponent
{
	GENERATED_BODY()

	UMetaGraphicsSettingsComponent();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	UMetaGraphicsSettingsWidget* GraphicsSettingsWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> GraphicsSettingWidgetClass;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="Meta Screen Setting")
	void ShowGraphicsSettingsUI();
	
	UFUNCTION(BlueprintCallable, Category="Meta Screen Setting")
	void HideGraphicsSettingsUI();
};