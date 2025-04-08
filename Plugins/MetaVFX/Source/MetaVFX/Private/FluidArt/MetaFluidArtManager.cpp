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

void UMetaFluidArtManager::NewColliderWithLiDAR(const int32 ID, const FVector2D Percentage)
{
	const FVector2D ScreenPosition = FVector2D(ViewportSize.X * Percentage.X, ViewportSize.Y * Percentage.Y);
	SetColliderLocation(ID, ScreenPosition);
}

void UMetaFluidArtManager::UpdateColliderWithLiDAR(const int32 ID, const FVector2D Percentage)
{
	const FVector2D ScreenPosition = FVector2D(ViewportSize.X * Percentage.X, ViewportSize.Y * Percentage.Y);
	SetColliderLocation(ID, ScreenPosition);
}

void UMetaFluidArtManager::RemoveColliderWithLiDAR(const int32 ID)
{
	if (AllocatedColliders.Contains(ID))
	{
		const float DecreaseStartTime = (1 - AllocatedColliders[ID].Actor->GetActorScale3D().X / InteractionScale) * ScaleDecreaseTime;
		const int32 DeallocID = GetNonDuplicatedDeallocateID();
		FluidCollidersToBeDeallocated.Emplace(DeallocID, AllocatedColliders[ID]);
		AllocatedColliders.Remove(ID);
		
		ColliderDecreaseScale(DeallocID, DecreaseStartTime);
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
		const float DecreaseStartTime = (1 - AllocatedColliders[MouseID].Actor->GetActorScale3D().X / InteractionScale) * ScaleDecreaseTime;
		const int32 DeallocID = GetNonDuplicatedDeallocateID();
		FluidCollidersToBeDeallocated.Emplace(DeallocID, AllocatedColliders[MouseID]);
		AllocatedColliders.Remove(MouseID);
		
		ColliderDecreaseScale(DeallocID, DecreaseStartTime);
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
			const float DecreaseStartTime = (1 - AllocatedColliders[ID].Actor->GetActorScale3D().X / InteractionScale) * ScaleDecreaseTime;
			const int32 DeallocID = GetNonDuplicatedDeallocateID();
			FluidCollidersToBeDeallocated.Emplace(DeallocID, AllocatedColliders[ID]);
			AllocatedColliders.Remove(ID);

			ColliderDecreaseScale(DeallocID, DecreaseStartTime);
		}
	}	
}

void UMetaFluidArtManager::ColliderIncreaseScale(const int32 ID, const float ElapsedTime)
{
	if (!AllocatedColliders.Contains(ID)) return;

	const float Alpha = ElapsedTime / ScaleIncreaseTime;
	const float Progress = EaseInOutQuad(Alpha);
	const float NewScale = FMath::Clamp(FMath::Lerp(0, InteractionScale, Progress), 0, InteractionScale);
	
	AllocatedColliders[ID].Actor->SetActorScale3D(FVector(NewScale));

	if (Alpha < 1)
	{
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("ColliderIncreaseScale"), ID, ElapsedTime + 0.0167f);
		GetWorld()->GetTimerManager().SetTimer(AllocatedColliders[ID].ScaleTimerHandle, TimerDel, 0.0167f, false);
	}
	else
	{
		AllocatedColliders[ID].Actor->SetActorScale3D(FVector(InteractionScale));
	}
}

void UMetaFluidArtManager::ColliderDecreaseScale(const int32 ID, const float ElapsedTime)
{
	if (!FluidCollidersToBeDeallocated.Contains(ID)) return;

	const float Alpha = ElapsedTime / ScaleIncreaseTime;
	const float Progress = EaseInOutQuad(Alpha);
	const float NewScale = FMath::Clamp(FMath::Lerp(InteractionScale, 0, Progress), 0, InteractionScale);
	
	FluidCollidersToBeDeallocated[ID].Actor->SetActorScale3D(FVector(NewScale));

	if (Alpha < 1)
	{
		if (FluidCollidersToBeDeallocated[ID].ScaleTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(FluidCollidersToBeDeallocated[ID].ScaleTimerHandle);
			FluidCollidersToBeDeallocated[ID].ScaleTimerHandle.Invalidate();
		}
		
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("ColliderDecreaseScale"), ID, ElapsedTime + 0.0167f);
		GetWorld()->GetTimerManager().SetTimer(FluidCollidersToBeDeallocated[ID].ScaleTimerHandle, TimerDel, 0.0167f, false);
	}
	else
	{
		FluidCollidersToBeDeallocated[ID].Actor->SetActorScale3D(FVector::Zero());
		MemoryPoolObject->DeallocateActor(FluidCollidersToBeDeallocated[ID].Actor, true);
	}	
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

float UMetaFluidArtManager::EaseInOutQuad(const float X) const
{
	return X < 0.5 ? 2 * X * X : 1 - FMath::Pow(-2 * X + 2, 2) / 2;
}