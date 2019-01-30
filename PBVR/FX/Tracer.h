// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#pragma once

#include "PBVR.h"
#include "GameFramework/Actor.h"
#include "Tracer.generated.h"

UCLASS()
class PBVR_API ATracer : public AActor
{
	GENERATED_BODY()
	
public:	
	ATracer();

	// AActor:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
		
private:
	///////////////////////// Components /////////////////////////
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneRootComponent;

	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneIntermediateComponent;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* TracerMeshComponent;

	UPROPERTY()
	class UMaterialInstanceDynamic* TracerMaterial;

	///////////////////////// Resources /////////////////////////
	UPROPERTY()
	class UMaterialInterface* M_Tracer;

	///////////////////////// State /////////////////////////
	float Alpha;
};
