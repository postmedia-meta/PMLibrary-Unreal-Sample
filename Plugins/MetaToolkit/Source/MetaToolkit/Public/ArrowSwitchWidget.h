// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ArrowSwitchWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedIndex, const int32, Index);

UCLASS()
class METATOOLKIT_API UArrowSwitchWidget : public UUserWidget
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
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void ClickPrevButton();
	
	UFUNCTION()
	void ClickNextButton();

public:
	UFUNCTION(BlueprintCallable)
	void SetCurrentIndex(const int32 Index);
	
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentIndex() { return CurrentIndex; }

	UFUNCTION(BlueprintCallable)
	FString GetCurrentStringFromTextBlock() { return TextBlock->GetText().ToString(); }
};
