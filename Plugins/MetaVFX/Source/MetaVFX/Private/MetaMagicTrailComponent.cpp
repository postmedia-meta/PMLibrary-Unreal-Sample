

#include "MetaMagicTrailComponent.h"

#include "MemoryPoolObject.h"
#include "MetaMagicTrailWidget.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

UMetaMagicTrailComponent::UMetaMagicTrailComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> MetaMagicTrailWidgetFinder(TEXT("/Script/UMGEditor.WidgetBlueprint'/MetaVFX/MagicTrail/UI/WBP_MetaMagicTrail.WBP_MetaMagicTrail_C'"));
	if (MetaMagicTrailWidgetFinder.Succeeded())
	{
		MagicTrailWidgetClass = MetaMagicTrailWidgetFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<ANiagaraActor> MagicTrailNiagaraActorFinder(TEXT("/Script/Engine.Blueprint'/MetaVFX/MagicTrail/VFX/MagicTrail/BP_MagicTrailNiagaraActor.BP_MagicTrailNiagaraActor_C'"));
	if (MagicTrailNiagaraActorFinder.Succeeded())
	{
		MagicTrailNiagaraActorClass = MagicTrailNiagaraActorFinder.Class;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> SpriteMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Sprite.M_Sprite'"));
	if (SpriteMaterialFinder.Succeeded())
	{
		DefaultSpriteMaterial = SpriteMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> TrailMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Trail.M_Trail'"));
	if (TrailMaterialFinder.Succeeded())
	{
		DefaultTrailMaterial = TrailMaterialFinder.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterial> CircleTrailMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Trail_Circle.M_Trail_Circle'"));
	if (CircleTrailMaterialFinder.Succeeded())
	{
		CircleTrailMaterial = CircleTrailMaterialFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> SquareTrailMaterialFinder(TEXT("/Script/Engine.Material'/MetaVFX/MagicTrail/VFX/MagicTrail/Materials/M_Trail_Square.M_Trail_Square'"));
	if (SquareTrailMaterialFinder.Succeeded())
	{
		SquareTrailMaterial = SquareTrailMaterialFinder.Object;
	}
}

void UMetaMagicTrailComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("This component can only be used as a component of a player controller."));
		return;
	}

	if (MagicTrailWidgetClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MagicTrailWidgetClass is nullptr"));
		return;
	}

	if (bUseEditUI)
	{
		MagicTrailWidget = CreateWidget<UMetaMagicTrailWidget>(PlayerController, MagicTrailWidgetClass);
		MagicTrailWidget->AddToViewport();
		MagicTrailWidget->HideWidget();
	}
	
	ChangeMaskShape(MaskShape);
	SpriteMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DefaultSpriteMaterial, nullptr);
	SpriteMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), Texture2D);
	
	MemoryPoolObject = NewObject<UMemoryPoolObject>(PlayerController, TEXT("Memory Pool"));
	MemoryPoolObject->SetActorClass(MagicTrailNiagaraActorClass);
	MemoryPoolObject->CreateActors(InitCreatePoolNum);

	if (bUseMouse)
	{
		// 액터의 InputComponent를 가져옵니다.
		if (UInputComponent* InputComp = PlayerController->FindComponentByClass<UInputComponent>())
		{
			InputComp->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &UMetaMagicTrailComponent::OnLeftMouseDown);
			InputComp->BindKey(EKeys::LeftMouseButton, IE_Released, this, &UMetaMagicTrailComponent::OnLeftMouseUp);
		}
	}

	FViewport::ViewportResizedEvent.AddUObject(this, &UMetaMagicTrailComponent::OnViewportResized);

	if (ViewportSize.X <= 0|| ViewportSize.Y <= 0)
	{
		PlayerController->GetViewportSize(ViewportSize.X, ViewportSize.Y);

		if (MagicTrailWidget)
		{
			MagicTrailWidget->SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);
			MagicTrailWidget->MetaCursorResizing();
		}
	}
}

void UMetaMagicTrailComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (MouseDelayHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
		MouseDelayHandle.Invalidate();
	}

	for (auto& LiDARActor : LiDARActors)
	{
		if (LiDARActor.Value.DelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(LiDARActor.Value.DelayHandle);
			LiDARActor.Value.DelayHandle.Invalidate();
		}
	}

	FViewport::ViewportResizedEvent.Clear();
}

void UMetaMagicTrailComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentTime += DeltaTime;
	if (CurrentTime > ResetTime)
	{
		CurrentTime = 0;
		DeallocateAllMagicTrails();
	}
	
	if (bIsDragging) LeftMouseDrag();
}

void UMetaMagicTrailComponent::NewMagicTrailsWithLiDAR(const TArray<int32> IDs, const TArray<FVector2D> Percentages)
{
	if (IDs.Num() != Percentages.Num()) return;
	
	for (int32 i = 0; i < IDs.Num(); ++i)
	{
		bool IsExist = false;
		for (auto& LiDARActor : LiDARActors)
		{
			if (LiDARActor.Key == IDs[i])
			{
				IsExist = true;
				break;
			}
		}
		
		if (!IsExist)
		{
			FLiDARActor LiDARActor;
			LiDARActor.Actor = MemoryPoolObject->AllocateActor().Actor;
			LiDARActor.NiagaraComponent = Cast<ANiagaraActor>(LiDARActor.Actor)->GetNiagaraComponent();
			InitNiagaraComponent(LiDARActor.NiagaraComponent);

			LiDARActors.Emplace(IDs[i], LiDARActor);
			SetLiDARActorLocationFromScreenPercentage(IDs[i], Percentages[i].X, Percentages[i].Y);
		}
	}
}

void UMetaMagicTrailComponent::UpdateMagicTrailsWithLiDAR(const TArray<int32> IDs, const TArray<FVector2D> Percentages)
{
	if (IDs.Num() != Percentages.Num()) return;
	
	for (int32 i = 0; i < IDs.Num(); ++i)
	{
		bool IsExist = false;
		for (auto& LiDARActor : LiDARActors)
		{
			if (LiDARActor.Key == IDs[i])
			{
				IsExist = true;
				SetLiDARActorLocationFromScreenPercentage(LiDARActor.Key, Percentages[i].X, Percentages[i].Y);
				break;
			}
		}
		
		if (!IsExist)
		{
			FLiDARActor LiDARActor;
			LiDARActor.Actor = MemoryPoolObject->AllocateActor().Actor;
			LiDARActor.NiagaraComponent = Cast<ANiagaraActor>(LiDARActor.Actor)->GetNiagaraComponent();
			InitNiagaraComponent(LiDARActor.NiagaraComponent);

			LiDARActors.Emplace(IDs[i],LiDARActor);
			SetLiDARActorLocationFromScreenPercentage(IDs[i], Percentages[i].X, Percentages[i].Y);	
		}
	}
}

void UMetaMagicTrailComponent::RemoveMagicTrailsWithLiDAR(const TArray<int32> IDs)
{
	for (int32 i = 0; i < IDs.Num(); ++i)
	{
		for (auto& LiDARActor : LiDARActors)
		{
			if (LiDARActor.Key == IDs[i])
			{
				if (LiDARActor.Value.DelayHandle.IsValid())
				{
					GetWorld()->GetTimerManager().ClearTimer(LiDARActor.Value.DelayHandle);
					LiDARActor.Value.DelayHandle.Invalidate();
				}
		
				LiDARActor.Value.NiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
				MemoryPoolObject->DeallocateActor(LiDARActor.Value.Actor, false);

				LiDARActor.Value.Actor = nullptr;
				LiDARActor.Value.NiagaraComponent = nullptr;

				LiDARActors.Remove(IDs[i]);
			}
		}
	}
}

void UMetaMagicTrailComponent::DeallocateAllMagicTrails()
{
	if (MouseActor)
	{
		if (MouseDelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
			MouseDelayHandle.Invalidate();
		}
		
		MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(MouseActor, false);

		MouseActor = nullptr;
		MouseNiagaraComponent = nullptr;
	}

	for (auto& LiDARActor : LiDARActors)
	{
		if (LiDARActor.Value.DelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(LiDARActor.Value.DelayHandle);
			LiDARActor.Value.DelayHandle.Invalidate();
		}
		
		LiDARActor.Value.NiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(LiDARActor.Value.Actor, false);

		LiDARActor.Value.Actor = nullptr;
		LiDARActor.Value.NiagaraComponent = nullptr;
	}
	
	LiDARActors.Empty();
}

void UMetaMagicTrailComponent::ShowWidget()
{
	if (MagicTrailWidget)
	{
		MagicTrailWidget->ShowWidget();
	}
}

void UMetaMagicTrailComponent::HideWidget()
{
	if (MagicTrailWidget)
	{
		MagicTrailWidget->HideWidget();
	}
}

void UMetaMagicTrailComponent::ChangeMaskShape(const EMaskShape Shape)
{
	if (TrailMaterialInstanceDynamic && MaskShape == Shape) return;

	MaskShape = Shape;
	switch (MaskShape)
	{
	case EMaskShape::Default:
		TrailMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DefaultTrailMaterial, nullptr);
		break;
	case EMaskShape::Circle:
		TrailMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(CircleTrailMaterial, nullptr);
		break;
	case EMaskShape::Square:
		TrailMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(SquareTrailMaterial, nullptr);
		break;
	}
	TrailMaterialInstanceDynamic->SetTextureParameterValue(TEXT("Texture"), Texture2D);
}

void UMetaMagicTrailComponent::OnViewportResized(FViewport* Viewport, unsigned int I)
{
	ViewportSize = Viewport->GetSizeXY();

	if (MagicTrailWidget)
	{
		MagicTrailWidget->SetSizeBoxSize(ViewportSize.X, ViewportSize.Y);
		MagicTrailWidget->MetaCursorResizing();
	}
}

void UMetaMagicTrailComponent::OnLeftMouseDown()
{
	bIsDragging = true;
	if (MouseActor == nullptr)
	{
		MouseActor = MemoryPoolObject->AllocateActor().Actor;
		MouseNiagaraComponent = Cast<ANiagaraActor>(MouseActor)->GetNiagaraComponent();
		InitNiagaraComponent(MouseNiagaraComponent);

		SetMouseActorLocation();
	}
}

void UMetaMagicTrailComponent::OnLeftMouseUp()
{
	bIsDragging = false;
	if (MouseActor)
	{
		if (MouseDelayHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
			MouseDelayHandle.Invalidate();
		}
		
		MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
		MemoryPoolObject->DeallocateActor(MouseActor, false);
	
		MouseActor = nullptr;
		MouseNiagaraComponent = nullptr;
	}
}

void UMetaMagicTrailComponent::LeftMouseDrag()
{
	if (MouseActor == nullptr)
	{
		MouseActor = MemoryPoolObject->AllocateActor().Actor;
		MouseNiagaraComponent = Cast<ANiagaraActor>(MouseActor)->GetNiagaraComponent();
		InitNiagaraComponent(MouseNiagaraComponent);
	}
	
	SetMouseActorLocation();
}

void UMetaMagicTrailComponent::InitNiagaraComponent(UNiagaraComponent* NiagaraComponent)
{
	NiagaraComponent->SetVariableMaterial(TEXT("MI_Sprite"), SpriteMaterialInstanceDynamic);
	NiagaraComponent->SetVariableMaterial(TEXT("MI_Trail"), TrailMaterialInstanceDynamic);
	NiagaraComponent->SetVariableFloat(TEXT("LifeTime"), LifeTime);
	NiagaraComponent->SetVariableFloat(TEXT("Scale"), Scale);
	NiagaraComponent->SetVariableFloat(TEXT("SpriteScale"), SpriteScale);
	NiagaraComponent->SetVariableFloat(TEXT("SpriteRateScale"), SpriteRateScale);
	NiagaraComponent->SetVariableFloat(TEXT("EdgeThickness"), EdgeThickness);
	NiagaraComponent->SetVariableFloat(TEXT("EdgeIntensity"), EdgeIntensity);
	NiagaraComponent->SetVariableLinearColor(TEXT("EdgeColor"), EdgeColor);
	NiagaraComponent->SetVariableLinearColor(TEXT("SpriteColor"), SpriteColor);
	NiagaraComponent->SetVariableBool(TEXT("bAutoColor"), bAutoColor);
}

void UMetaMagicTrailComponent::SetMouseActorLocation()
{
	if (bUseRay)
	{
		FHitResult HitResult;
		if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
		{
			MouseActor->SetActorLocation(HitResult.Location);
			if (!MouseDelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(MouseDelayHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
				}), ParticleActivationThresholdSec, false);		
			}
		}
		else
		{
			if (MouseDelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().ClearTimer(MouseDelayHandle);
				MouseDelayHandle.Invalidate();

				MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
			}
		}
	}
	else
	{
		FVector WorldPos = FVector::Zero();
		FVector WorldDir = FVector::Zero();
		if (PlayerController->DeprojectMousePositionToWorld(WorldPos, WorldDir))
		{
			MouseActor->SetActorLocation(WorldPos);
			if (!MouseDelayHandle.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(MouseDelayHandle, FTimerDelegate::CreateLambda([this]()->void
				{
					MouseNiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
				}), ParticleActivationThresholdSec, false);		
			}
		}
	}
}

void UMetaMagicTrailComponent::SetLiDARActorLocationFromScreenPercentage(const int32 ID, const float X, const float Y)
{
	FLiDARActor* LiDARActor = LiDARActors.Find(ID);
	const FVector2D ScreenPosition = FVector2D(ViewportSize.X * X, ViewportSize.Y * Y);

	if(LiDARActor)
	{
		if (bUseRay)
		{
			FHitResult HitResult;
			if (PlayerController->GetHitResultAtScreenPosition(ScreenPosition, ECC_Visibility, false, HitResult))
			{
				LiDARActor->Actor->SetActorLocation(HitResult.Location);
				if (!LiDARActor->DelayHandle.IsValid())
				{
					GetWorld()->GetTimerManager().SetTimer(LiDARActor->DelayHandle, FTimerDelegate::CreateLambda([this, ID]()->void
					{
						if (const FLiDARActor* LiDARActor = LiDARActors.Find(ID))
						{
							LiDARActor->NiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
						}
					}), ParticleActivationThresholdSec, false);		
				}
			}
			else
			{
				if (LiDARActor->DelayHandle.IsValid())
				{
					GetWorld()->GetTimerManager().ClearTimer(LiDARActor->DelayHandle);
					LiDARActor->DelayHandle.Invalidate();

					LiDARActor->NiagaraComponent->SetVariableFloat(TEXT("RateScale"), 0);
				}
			}
		}
		else
		{
			FVector WorldPos = FVector::Zero();
			FVector WorldDir = FVector::Zero();
			if (UGameplayStatics::DeprojectScreenToWorld(PlayerController, ScreenPosition, WorldPos, WorldDir))
			{
				LiDARActor->Actor->SetActorLocation(WorldPos);
				if (!LiDARActor->DelayHandle.IsValid())
				{
					GetWorld()->GetTimerManager().SetTimer(LiDARActor->DelayHandle, FTimerDelegate::CreateLambda([this, ID]()->void
					{
						if (const FLiDARActor* LiDARActor = LiDARActors.Find(ID))
						{
							LiDARActor->NiagaraComponent->SetVariableFloat(TEXT("RateScale"), RateScale);
						}
					}), ParticleActivationThresholdSec, false);		
				}
			}
		}	
	}
}

bool UMetaMagicTrailComponent::IsShowWidget()
{
	if (MagicTrailWidget == nullptr) return false;

	return MagicTrailWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible;
}
