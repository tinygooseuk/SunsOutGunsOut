// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/Actor.h"
#include "UMG3DWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVRWidgetShotSignature);

UCLASS()
class PBVR_API AUMG3DWidget : public AActor
{
	GENERATED_BODY()
	
public:	
	AUMG3DWidget();

	// AActor:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	///////////////////////// Events /////////////////////////
	// Widget was shot with a gun
	UPROPERTY(BlueprintAssignable)
	FVRWidgetShotSignature OnWidgetShot;

	///////////////////////// Editables /////////////////////////
	// Width of widget
	UPROPERTY(EditAnywhere)
	float WidgetWidth;

	// Height of widget
	UPROPERTY(EditAnywhere)
	float WidgetHeight;

	// Widget class to use
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetClass;

	// How much to bob up/down
	UPROPERTY(EditAnywhere)
	float BobAmount;

	// How fast to bob up/down
	UPROPERTY(EditAnywhere)
	float BobSpeed;
	
private:
	///////////////////////// Components /////////////////////////
	UPROPERTY()
	class USceneComponent* SceneComponent;
	UPROPERTY()
	class UWidgetComponent* WidgetComponent;

	///////////////////////// Resources /////////////////////////
	class UMaterialInstanceConstant* M_Widget3DPassThrough_Translucent_OneSided;
};
