// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/Actor.h"
#include "TimerClock.generated.h"

UCLASS()
class PBVR_API ATimerClock : public AActor
{
	GENERATED_BODY()
	
public:	
	ATimerClock();

	// AActor:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	///////////////////////// Components /////////////////////////
	UPROPERTY()
	class UStaticMeshComponent* FaceMeshComponent;

	UPROPERTY()
	class USceneComponent* HandOffsetComponent;

	UPROPERTY()
	class UStaticMeshComponent* HandMeshComponent;
};
