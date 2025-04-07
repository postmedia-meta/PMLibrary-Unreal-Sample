// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MetaFluidArtManager.generated.h"

class UMetaFluidArtWidget;
class UMetaFluidArtActorComponent;
class UMemoryPoolObject;

USTRUCT(Blueprintable)
struct FFluidCollider
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Actor;

	UPROPERTY()
	FTimerHandle ScaleTimerHandle;
};

UCLASS( ClassGroup="MetaVFX", meta=(BlueprintSpawnableComponent) )
class METAVFX_API UMetaFluidArtManager : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UUserWidget> FluidArtWidgetClass;

	UPROPERTY()
	UMetaFluidArtWidget* FluidArtWidget;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere)
	TArray<UMetaFluidArtActorComponent*> MetaFluidArtActorComponents;

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AActor> InteractionCollisionClass;
	
public:	
	UMetaFluidArtManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MetaFluidArtManager")
	UMemoryPoolObject* MemoryPoolObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtManager")
	int32 InitCreatePoolNum = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtManager")
	float InteractionScale = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtManager")
	float ScaleIncreaseTime = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtManager")
	float ScaleDecreaseTime = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MetaFluidArtManager")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic) };
	
private:
	float CurrentTime = 0;

	bool bIsDragging = false;

	FIntPoint ViewportSize;

	// UPROPERTY()
	// TMap<int32, FLiDARActor> LiDARActors;

	int32 MouseID = MIN_int32;
	
	TMap<int32, FFluidCollider> AllocatedColliders;
	TMap<int32, FFluidCollider> FluidCollidersToBeDeallocated;

	UPROPERTY()
	UTexture2D* TextureFromFile;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="MetaFluidArt")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category="MetaFluidArt")
	void HideWidget();

	UFUNCTION(BlueprintPure, Category="MetaFluidArt")
	bool IsShowWidget() const;

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void NewCollidersWithLiDAR(const int32 ID, const FVector2D Percentage);

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void UpdateCollidersWithLiDAR(const int32 ID, const FVector2D Percentage);

	UFUNCTION(BlueprintCallable, Category="MetaMagicTrail")
	void RemoveCollidersWithLiDAR(const int32 ID);

private:
	void OnViewportResized(FViewport* Viewport, unsigned int I);

	UFUNCTION()
	void OnLeftMouseDown();

	UFUNCTION()
	void OnLeftMouseUp();

	UFUNCTION()
	void SetColliderLocation(const int32 ID, const FVector2D ScreenPosition = FVector2D::Zero());

	UFUNCTION()
	void ColliderIncreaseScale(const int32 ID);

	UFUNCTION()
	void ColliderDecreaseScale(const int32 ID);

	UFUNCTION()
	void InitLiDARAllocatedActor(const int32 ID);

	UFUNCTION()
	int32 GetNonDuplicatedDeallocateID() const;
};
