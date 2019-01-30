// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "Tracer.h"

// Engine
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ATracer::ATracer()
	: Alpha(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	// Resources
	UStaticMesh* SM_Cylinder = FIND_RESOURCE(StaticMesh, SM_Cylinder, "StaticMeshes");
	M_Tracer = FIND_RESOURCE(Material, M_Tracer, "Materials");

	// Create root
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRootComponent);

	SceneIntermediateComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneIntermediate"));
	SceneIntermediateComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	SceneIntermediateComponent->SetupAttachment(SceneRootComponent);

	// Create mesh
	TracerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TracerMesh"));
	TracerMeshComponent->SetStaticMesh(SM_Cylinder);
	TracerMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TracerMeshComponent->SetRelativeScale3D(FVector(0.025f, 0.025f, 10.0f));
	TracerMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	TracerMeshComponent->SetRelativeLocation(FVector(0.0f, +5'00.0f, 0.0f));
	TracerMeshComponent->SetupAttachment(SceneIntermediateComponent);
}

void ATracer::BeginPlay()
{
	Super::BeginPlay();
	
	// Create material
	TracerMaterial = UMaterialInstanceDynamic::Create(M_Tracer, this);
	TracerMeshComponent->SetMaterial(0, TracerMaterial);
}
	
void ATracer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Alpha -= 2.0f * DeltaTime;

	if (Alpha <= 0.0f)
	{
		Destroy();
	}
	else
	{
		static FName NAME_Alpha = TEXT("Alpha");
		TracerMaterial->SetScalarParameterValue(NAME_Alpha, Alpha);
	}
}

