// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/Actor.h"
#include "Duck.generated.h"

UCLASS()
class PBVR_API ADuck : public AActor
{
	GENERATED_BODY()
	
public:	
	ADuck();

	// AActor:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	// Explode a duck into a million pieces - and potentially award points
	void Explode(bool bAwardPoints);

	///////////////////////// State /////////////////////////
	bool bExploded;

	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class USoundWave* Sound_Quack;

	///////////////////////// Components /////////////////////////
	UPROPERTY()
	class UStaticMeshComponent* StaticMeshComponent;
};
