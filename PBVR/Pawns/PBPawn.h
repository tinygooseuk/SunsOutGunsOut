// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/Pawn.h"
#include "PBPawn.generated.h"

#define Ammo_Infinite	(-1)

UCLASS()
class PBVR_API APBPawn : public APawn
{
	GENERATED_BODY()

public:
	APBPawn();

	// AActor:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	///////////////////////// Inputs /////////////////////////
	// Fire the weapon
	UFUNCTION()
	void Fire();

	// Switch on/off laser sight
	UFUNCTION()
	void ToggleLaserSight();

	///////////////////////// Getters/Setters /////////////////////////
	// Set the new ammo amount
	UFUNCTION(BlueprintCallable)
	void SetAmmo(int32 InAmmoAmount); // Set the amount of ammo in the gun. Use -1 for infinite.

	// Set the laser sight on/off
	UFUNCTION(BlueprintCallable)
	void SetLaserSightOn(bool bInLaserSightOn);

	// Get the laser sight state
	UFUNCTION(BlueprintCallable)
	bool GetLaserSightOn() const;

private:
	///////////////////////// Components /////////////////////////
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	class UMotionControllerComponent* RightControllerComponent;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* GunMeshComponent;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* GunAmmoRingComponent;

	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* GunAmmoTextComponent;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* LaserIntermediateComponent;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* LaserMeshComponent;

	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class USoundWave* Sound_Shoot;
	UPROPERTY()
	class USoundWave* Sound_EmptyClip;
	UPROPERTY()
	class USoundWave* Sound_LaserSightOn;
	UPROPERTY()
	class USoundWave* Sound_LaserSightOff;

	UPROPERTY()
	class UMaterialInstanceDynamic* MI_SoftNeon;

	UPROPERTY()
	class UMaterialInstanceConstant* MI_LaserSight;

	UPROPERTY()
	class UHapticFeedbackEffect_Base* Haptic_FireGun;

	UPROPERTY()
	class UHapticFeedbackEffect_Base* Haptic_LaserSightToggle;

	///////////////////////// State /////////////////////////
	int32 Ammo = -1;
};
