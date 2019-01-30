// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/Actor.h"
#include "Bomb.generated.h"

UCLASS()
class PBVR_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	ABomb();

	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Static method to spawn an explosion at a given position & scale
	static void SpawnExplosion(UObject* WorldContextObject, const FVector& WorldPosition, float Scale = 5.0f);

private:
	// Explode bomb - deduct score if award points is true
	void Explode(bool bAwardPoints);

	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class USoundWave* Sound_Bang;

	///////////////////////// Components /////////////////////////
	UPROPERTY()
	class UStaticMeshComponent* StaticMeshComponent;
};
