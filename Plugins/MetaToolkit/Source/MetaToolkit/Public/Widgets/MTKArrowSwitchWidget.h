// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MTKArrowSwitchWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedIndex, const int32, Index);

UCLASS()
class METATOOLKIT_API UMTKArrowSwitchWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="ArrowSwitchWidget")
	UTextBlock* TextBlock;
    	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="ArrowSwitchWidget")
	UButton* PrevButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="ArrowSwitchWidget")
	UButton* NextButton;

private:
	int32 CurrentIndex = 0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="Element")
	TArray<FString> Elements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget), Category="Element")
	int32 StartIndex = 0;

	UPROPERTY(BlueprintAssignable, Category="ArrowSwitchWidget|Event")
	FOnChangedIndex OnChangedIndex;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void ClickPrevButton();
	
	UFUNCTION()
	void ClickNextButton();

public:
	UFUNCTION(BlueprintCallable, Category="ArrowSwitchWidget")
	void SetCurrentIndex(const int32 Index);
	
	UFUNCTION(BlueprintPure, Category="ArrowSwitchWidget")
	int32 GetCurrentIndex() { return CurrentIndex; }

	UFUNCTION(BlueprintPure, Category="ArrowSwitchWidget")
	FString GetCurrentStringFromTextBlock() { return TextBlock->GetText().ToString(); }
};
