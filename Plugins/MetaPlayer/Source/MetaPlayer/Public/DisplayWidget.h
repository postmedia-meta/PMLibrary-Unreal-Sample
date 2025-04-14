// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DisplayWidget.generated.h"

class UImage;
class UMediaTexture;
class UMaterial;

UCLASS()
class METAPLAYER_API UDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDisplayWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="Display")
	UPanelWidget* DisplayPanel;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Display")
	TArray<UImage*> Displays;

private:
	UPROPERTY(EditAnywhere, Category="Display|Material")
	UMaterial* DisplayMaterial;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> MaterialInstanceDynamics;
	
protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category="DisplayWidget")
	void SetMediaTexturesDisplayMaterials(const TArray<UMediaTexture*>& MediaTextures);
};
