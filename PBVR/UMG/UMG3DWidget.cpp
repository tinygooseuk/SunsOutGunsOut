// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "UMG3DWidget.h"

// Engine
#include "Components/WidgetComponent.h"
#include "Materials/MaterialInstanceConstant.h"

AUMG3DWidget::AUMG3DWidget()
	: WidgetWidth(250.0)
	, WidgetHeight(200.0f)
	, WidgetClass(nullptr)
	, BobAmount(10.0f)
	, BobSpeed(0.5f)
{
	PrimaryActorTick.bCanEverTick = true;

	// Resources 
	M_Widget3DPassThrough_Translucent_OneSided = FIND_ENGINE_RESOURCE(MaterialInstanceConstant, Widget3DPassThrough_Translucent_OneSided, "EngineMaterials");

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(SceneComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetupAttachment(GetRootComponent());
}

void AUMG3DWidget::BeginPlay()
{
	Super::BeginPlay();	
	
	
}

void AUMG3DWidget::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	WidgetComponent->SetWidgetClass(WidgetClass);
	WidgetComponent->InitWidget();
	WidgetComponent->SetMaterial(0, M_Widget3DPassThrough_Translucent_OneSided);
	WidgetComponent->SetDrawSize(FVector2D(WidgetWidth, WidgetHeight));
}

void AUMG3DWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BobAmount > 0.0f)
	{
		UWorld* World = GetWorld();
		GOOSE_BAIL(World);

		float TimeValue = FMath::Fmod(World->GetTimeSeconds() * BobSpeed, 1.0f);
		float OffsetZ = FMath::Sin(TimeValue * 2 * PI) * BobAmount;

		WidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, OffsetZ));
	}

}

float AUMG3DWidget::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float Original = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	OnWidgetShot.Broadcast();

	return Original;
}
