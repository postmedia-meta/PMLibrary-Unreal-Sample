// Fill out your copyright notice in the Description page of Project Settings.


#include "MemoryPoolObject.h"

#include "GameFramework/Actor.h"
#include "Engine/World.h"

FAllocatedActor UMemoryPoolObject::AllocateActor()
{
	if (ActorClass == nullptr) return FAllocatedActor(nullptr, 0);

	int32 Index;
	for (Index = 0; Index < MemoryPoolActors.Num(); ++Index)
	{
		if (!MemoryPoolActors[Index].bIsAllocated)
		{
			MemoryPoolActors[Index].Actor->SetActorHiddenInGame(false);
			MemoryPoolActors[Index].bIsAllocated = true;
			break;
		}
	}
	if (Index >= MemoryPoolActors.Num())CreateActors(10);
	
	return FAllocatedActor(MemoryPoolActors[Index].Actor, Index);
}

void UMemoryPoolObject::DeallocateActor(const AActor* Actor, const bool IsHide)
{
	if (ActorClass == nullptr) return;

	for (auto& MemoryPoolActor : MemoryPoolActors)
	{
		if (MemoryPoolActor.Actor == Actor)
		{
			if (IsHide)
			{
				MemoryPoolActor.Actor->SetActorHiddenInGame(true);
				MemoryPoolActor.Actor->SetActorLocation(FVector(0,0,-1000));	
			}
			MemoryPoolActor.bIsAllocated = false;
		}
	}
}

void UMemoryPoolObject::DeallocateActorByIndex(const int32 Index, const bool IsHide)
{
	if (ActorClass == nullptr) return;
	if (Index < 0 || Index >= MemoryPoolActors.Num()) return;

	if (IsHide)
	{
		MemoryPoolActors[Index].Actor->SetActorHiddenInGame(true);
		MemoryPoolActors[Index].Actor->SetActorLocation(FVector(0,0,-1000));	
	}
	MemoryPoolActors[Index].bIsAllocated = false;
}

void UMemoryPoolObject::DeallocateAllActors()
{
	if (ActorClass == nullptr) return;
	
	for (int i = 0; i < MemoryPoolActors.Num(); ++i)
	{
		if (MemoryPoolActors[i].bIsAllocated) DeallocateActorByIndex(i, true);
	}
}

void UMemoryPoolObject::CreateActors(const int32 Num)
{
	if (ActorClass == nullptr) return;
	
	for (int i = 0; i < Num; ++i)
	{
		FMemoryPoolActor NewMemoryPoolActor;
		NewMemoryPoolActor.Actor = GetWorld()->SpawnActor(ActorClass);
		NewMemoryPoolActor.Actor->SetActorHiddenInGame(true);
		NewMemoryPoolActor.Actor->SetActorLocation(FVector(0,0,-1000));
		
		MemoryPoolActors.Emplace(NewMemoryPoolActor);
	}
}