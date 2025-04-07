// Fill out your copyright notice in the Description page of Project Settings.


#include "FluidArt/MetaFluidArtManager.h"

#include "MemoryPoolObject.h"
#include "Blueprint/UserWidget.h"
#include "FluidArt/MetaFluidArtActorComponent.h"
#include "FluidArt/MetaFluidArtWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UMetaFluidArtManager::UMetaFluidArtManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> MetaFluidArtWidgetClassFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/MetaVFX/FluidArt/UI/WBP_MetaFluidArt.WBP_MetaFluidArt_C'"));
	if (MetaFluidArtWidgetClassFinder.Succeeded())
	{
		FluidArtWidgetClass = MetaFluidArtWidgetClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> InteractionCollisionClassFinder(TEXT("/Script/Engine.Blueprint'/MetaVFX/FluidArt/Blueprints/BP_Collider.BP_Collider_C'"));
	if (InteractionCollisionClassFinder.Succeeded())
	{
		InteractionCollisionClass = InteractionCollisionClassFinder.Class;
	}
}

// Called when the game starts
void UMetaFluidArtManager::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("This component can only be used as a component of a player controller."));
		return;
	}

	if (FluidArtWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("FluidArtWidgetClass is nullptr"));
		return;
	}

	TArray<AActor*> FluidArtActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("FluidArtActor"), FluidArtActors);
	for (const AActor* FluidArtActor : FluidArtActors)
	{
		if (UMetaFluidArtActorComponent* MetaFluidArtActorComponent = Cast<UMetaFluidArtActorComponent>(FluidArtActor->GetComponentByClass(UMetaFluidArtActorComponent::StaticClass())))
		{
			MetaFluidArtActorComponents.Emplace(MetaFluidArtActorComponent);
		}
	}

	FluidArtWidget = CreateWidget<UMetaFluidArtWidget>(PlayerController, FluidArtWidgetClass);
	FluidArtWidget->AddToViewport();
	FluidArtWidget->HideWidget();
	
	if (ViewportSize.X <= 0|| ViewportSize.Y <= 0)
	{
		PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);

		if (FluidArtWidget)
		{
			FluidArtWidget->SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);
			FluidArtWidget->MetaCursorResizing();
		}
	}

	MemoryPoolObject = NewObject<UMemoryPoolObject>(PlayerController, TEXT("Memory Pool"));
	MemoryPoolObject->SetActorClass(InteractionCollisionClass);
	MemoryPoolObject->CreateActors(InitCreatePoolNum);

	// 액터의 InputComponent를 가져옵니다.
	if (UInputComponent* InputComp = PlayerController->FindComponentByClass<UInputComponent>())
	{
		InputComp->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &UMetaFluidArtManager::OnLeftMouseDown);
		InputComp->BindKey(EKeys::LeftMouseButton, IE_Released, this, &UMetaFluidArtManager::OnLeftMouseUp);
	}

	FViewport::ViewportResizedEvent.AddUObject(this, &UMetaFluidArtManager::OnViewportResized);
}


// Called every frame
void UMetaFluidArtManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsDragging) SetColliderLocation(MouseID);
}

void UMetaFluidArtManager::ShowWidget()
{
	if (FluidArtWidget)
	{
		FluidArtWidget->ShowWidget();
	}
}

void UMetaFluidArtManager::HideWidget()
{
	if (FluidArtWidget)
	{
		FluidArtWidget->HideWidget();
	}
}

bool UMetaFluidArtManager::IsShowWidget() const
{
	if (FluidArtWidget == nullptr) return false;

	return FluidArtWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible;
}

void UMetaFluidArtManager::NewCollidersWithLiDAR(const int32 ID, const FVector2D Percentage)
{
	const FVector2D ScreenPosition = FVector2D(ViewportSize.X * Percentage.X, ViewportSize.Y * Percentage.Y);
	SetColliderLocation(ID, ScreenPosition);
}

void UMetaFluidArtManager::UpdateCollidersWithLiDAR(const int32 ID, const FVector2D Percentage)
{
	const FVector2D ScreenPosition = FVector2D(ViewportSize.X * Percentage.X, ViewportSize.Y * Percentage.Y);
	SetColliderLocation(ID, ScreenPosition);
}

void UMetaFluidArtManager::RemoveCollidersWithLiDAR(const int32 ID)
{
	if (AllocatedColliders.Contains(ID))
	{
		const int32 DeallocID = GetNonDuplicatedDeallocateID();
		FluidCollidersToBeDeallocated.Emplace(DeallocID, AllocatedColliders[ID]);
		AllocatedColliders.Remove(ID);
		ColliderDecreaseScale(DeallocID);
	}
}

void UMetaFluidArtManager::OnViewportResized(FViewport* Viewport, unsigned int I)
{
	ViewportSize = Viewport->GetSizeXY();

	if (FluidArtWidget)
	{
		FluidArtWidget->SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);
		FluidArtWidget->MetaCursorResizing();
	}
}

void UMetaFluidArtManager::OnLeftMouseDown()
{
	bIsDragging = true;
	SetColliderLocation(MouseID);
}

void UMetaFluidArtManager::OnLeftMouseUp()
{
	bIsDragging = false;
	if (AllocatedColliders.Contains(MouseID))
	{
		const int32 DeallocID = GetNonDuplicatedDeallocateID();
		FluidCollidersToBeDeallocated.Emplace(DeallocID, AllocatedColliders[MouseID]);
		AllocatedColliders.Remove(MouseID);
		ColliderDecreaseScale(DeallocID);
	}
}

void UMetaFluidArtManager::SetColliderLocation(const int32 ID, const FVector2D ScreenPosition)
{
	bool IsHit;
	FHitResult HitResult;
	if (ID == MouseID)
	{
		IsHit = PlayerController->GetHitResultUnderCursorForObjects(ObjectTypes, false, HitResult);
	}
	else
	{
		IsHit = PlayerController->GetHitResultAtScreenPosition(ScreenPosition, ObjectTypes, false, HitResult);
	}
	
	if (IsHit)
	{
		if (!AllocatedColliders.Contains(ID))
		{
			FFluidCollider FluidCollider;
			FluidCollider.Actor = MemoryPoolObject->AllocateActor().Actor;
			FluidCollider.Actor->SetActorHiddenInGame(true);
			FluidCollider.Actor->SetActorScale3D(FVector::Zero());
			AllocatedColliders.Emplace(ID, FluidCollider);
			
			ColliderIncreaseScale(ID);
		}
		
		AllocatedColliders[ID].Actor->SetActorLocation(HitResult.Location);
	}
	else
	{
		if (AllocatedColliders.Contains(ID))
		{
			const int32 DeallocID = GetNonDuplicatedDeallocateID();
			FluidCollidersToBeDeallocated.Emplace(DeallocID, AllocatedColliders[ID]);
			AllocatedColliders.Remove(ID);
			ColliderDecreaseScale(DeallocID);
		}
	}	
}

void UMetaFluidArtManager::ColliderIncreaseScale(const int32 ID)
{
	if (!AllocatedColliders.Contains(ID)) return;

	const float AddAmount = InteractionScale / ScaleIncreaseTime * 0.033f;
	float NewScale = AllocatedColliders[ID].Actor->GetActorScale3D().X + AddAmount;
	if (NewScale > InteractionScale) NewScale = InteractionScale;
	AllocatedColliders[ID].Actor->SetActorScale3D(FVector(NewScale));
	
	if (AllocatedColliders[ID].Actor->GetActorScale3D().X < InteractionScale)
	{
		if (AllocatedColliders[ID].ScaleTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(AllocatedColliders[ID].ScaleTimerHandle);
			AllocatedColliders[ID].ScaleTimerHandle.Invalidate();
		}

		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("ColliderIncreaseScale"), ID);
		GetWorld()->GetTimerManager().SetTimer(AllocatedColliders[ID].ScaleTimerHandle, TimerDel, 0.033f, false);
	}
}

void UMetaFluidArtManager::ColliderDecreaseScale(const int32 ID)
{
	if (!FluidCollidersToBeDeallocated.Contains(ID)) return;
		
	const float SubAmount = InteractionScale / ScaleDecreaseTime * 0.033f;
	float NewScale = FluidCollidersToBeDeallocated[ID].Actor->GetActorScale3D().X - SubAmount;
	if (NewScale < 0) NewScale = 0;
	FluidCollidersToBeDeallocated[ID].Actor->SetActorScale3D(FVector(NewScale));
	
	if (FluidCollidersToBeDeallocated[ID].Actor->GetActorScale3D().X > 0)
	{
		if (FluidCollidersToBeDeallocated[ID].ScaleTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(FluidCollidersToBeDeallocated[ID].ScaleTimerHandle);
			FluidCollidersToBeDeallocated[ID].ScaleTimerHandle.Invalidate();
		}
		
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("ColliderDecreaseScale"), ID);
		GetWorld()->GetTimerManager().SetTimer(FluidCollidersToBeDeallocated[ID].ScaleTimerHandle, TimerDel, 0.033f, false);
	}
	else
	{
		MemoryPoolObject->DeallocateActor(FluidCollidersToBeDeallocated[ID].Actor, true);
	}	
}

void UMetaFluidArtManager::InitLiDARAllocatedActor(const int32 ID)
{
	// MouseCollider.Actor = MemoryPoolObject->AllocateActor().Actor;
	// MouseCollider.Actor->SetActorHiddenInGame(true);
	// MouseActor->SetActorScale3D(FVector::Zero());
	// GetWorld()->GetTimerManager().SetTimer(MouseActorScaleHandle, FTimerDelegate::CreateLambda([this]()->void
	// {
	// 	if (MouseActor->GetActorScale3D().X < InteractionScale)
	// 	{
	// 		const float AddAmount = (InteractionScale / ScaleIncreaseTime) * 0.033f;
	// 		float NewScale = MouseActor->GetActorScale3D().X + AddAmount;
	// 		if (NewScale > InteractionScale) NewScale = InteractionScale;
	// 				
	// 		MouseActor->SetActorScale3D(FVector(NewScale));
	// 	}
	// }), 0.033f, true);
}

int32 UMetaFluidArtManager::GetNonDuplicatedDeallocateID() const
{
	int32 ID = MIN_int32;
	while (FluidCollidersToBeDeallocated.Contains(ID))
	{
		++ID;
	}

	return ID;
}
