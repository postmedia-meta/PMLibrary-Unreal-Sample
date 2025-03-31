// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MemoryPoolObject.generated.h"

USTRUCT(Blueprintable)
struct FMemoryPoolActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MemoryPoolActor")
	AActor* Actor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MemoryPoolActor")
	bool bIsAllocated = false;
};

USTRUCT(Blueprintable)
struct FAllocatedActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MemoryPoolActor")
	AActor* Actor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MemoryPoolActor")
	int32 Index = 1;
};

UCLASS()
class METAVFX_API UMemoryPoolObject : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="MemoryPoolObject")
	TArray<FMemoryPoolActor> MemoryPoolActors;
	
	UPROPERTY(VisibleAnywhere, Category="MemoryPoolObject")
	UClass* ActorClass;
	
public:
	UFUNCTION(BlueprintCallable, Category="MemoryPoolObject")
	FAllocatedActor AllocateActor();

	UFUNCTION(BlueprintCallable, Category="MemoryPoolObject")
	void DeallocateActor(const AActor* Actor, const bool IsHide);

	UFUNCTION(BlueprintCallable, Category="MemoryPoolObject")
	void DeallocateActorByIndex(const int32 Index, const bool IsHide);

	UFUNCTION(BlueprintCallable, Category="MemoryPoolObject")
	void DeallocateAllActors();

	UFUNCTION(BlueprintCallable, Category="MemoryPoolObject")
	void CreateActors(const int32 Num);

	// Getter, Setter
	void SetActorClass(UClass* PoolActorClass) {ActorClass = PoolActorClass;}

	UFUNCTION(BlueprintGetter, Category="MemoryPoolObject")
	UClass* GetActorClass() const {return ActorClass;}

	UFUNCTION(BlueprintGetter, Category="MemoryPoolObject")
	int32 GetTotalNum() const {return MemoryPoolActors.Num();}
};