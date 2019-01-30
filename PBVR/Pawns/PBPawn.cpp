// (C) 2019 TinyGoose Ltd., All Rights Reserved.

#include "PBPawn.h"

// Engine
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
	#include "GameFramework/PlayerController.h"
#include "Haptics/HapticFeedbackEffect_Curve.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Sound/SoundWave.h"

// Game
#include "FX/Tracer.h"

namespace
{
	static FName NAME_Muzzle = TEXT("Muzzle");
}

APBPawn::APBPawn()	
	: Ammo(Ammo_Infinite)
{
	PrimaryActorTick.bCanEverTick = true;

	// Get resources
	UStaticMesh* SM_Gun = FIND_RESOURCE(StaticMesh, SM_Gun, "StaticMeshes");
	UStaticMesh* SM_GunRing = FIND_RESOURCE(StaticMesh, SM_GunRing, "StaticMeshes");
	UStaticMesh* SM_Cylinder = FIND_RESOURCE(StaticMesh, SM_Cylinder, "StaticMeshes");

	Sound_Shoot = FIND_RESOURCE(SoundWave, Shoot, "Sounds");
	Sound_EmptyClip = FIND_RESOURCE(SoundWave, EmptyClip, "Sounds");
	Sound_LaserSightOn = FIND_RESOURCE(SoundWave, LaserSightOn, "Sounds");
	Sound_LaserSightOff = FIND_RESOURCE(SoundWave, LaserSightOff, "Sounds");

	MI_LaserSight = FIND_RESOURCE(MaterialInstanceConstant, MI_LaserSight, "Materials");

	Haptic_FireGun = FIND_RESOURCE(HapticFeedbackEffect_Curve, Haptic_FireGun, "FX");
	Haptic_LaserSightToggle = FIND_RESOURCE(HapticFeedbackEffect_Curve, Haptic_LaserSightToggle, "FX");

	// Create camera component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("VRCamera"));
	CameraComponent->SetupAttachment(GetRootComponent());

	// Right hand
	RightControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("GunHandController"));
	RightControllerComponent->SetTrackingMotionSource(TEXT("Right"));
	RightControllerComponent->SetupAttachment(GetRootComponent());

	GunMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	GunMeshComponent->SetStaticMesh(SM_Gun);
	GunMeshComponent->SetRelativeRotation(FRotator(+10.0f, 0.0f, 0.0f));
	GunMeshComponent->SetupAttachment(RightControllerComponent);

	// Gun UI
	GunAmmoTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("GunAmmo"));
	GunAmmoTextComponent->SetRelativeLocation(FVector(3.0f, 0.0f, 8.0f));
	GunAmmoTextComponent->SetRelativeRotation(FRotator(60.0f, -180.0f, 90.0f));
	GunAmmoTextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	GunAmmoTextComponent->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	GunAmmoTextComponent->SetWorldSize(4.0f);
	GunAmmoTextComponent->SetText(FText::FromString(TEXT("8")));
	GunAmmoTextComponent->SetupAttachment(GunMeshComponent);

	GunAmmoRingComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunRing"));
	GunAmmoRingComponent->SetRelativeLocation(FVector(3.0f, 0.0f, 8.0f));
	GunAmmoRingComponent->SetRelativeRotation(FRotator(-5.0f, 95.0f, 25.0f));
	GunAmmoRingComponent->SetWorldScale3D(FVector(0.08f, 0.08f, 0.08f));
	GunAmmoRingComponent->SetStaticMesh(SM_GunRing);
	GunAmmoRingComponent->SetupAttachment(GunMeshComponent);

	// Laser sight
	FTransform SocketTransform = GunMeshComponent->GetSocketTransform(NAME_Muzzle, ERelativeTransformSpace::RTS_Actor);

	LaserIntermediateComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneIntermediate"));
	LaserIntermediateComponent->SetRelativeRotation(FRotator(210.0f, 0.0f, -90.0f));
	LaserIntermediateComponent->SetRelativeLocation(SocketTransform.GetLocation());
	LaserIntermediateComponent->SetupAttachment(GunMeshComponent);

	// Create mesh
	LaserMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TracerMesh"));
	LaserMeshComponent->SetStaticMesh(SM_Cylinder);
	LaserMeshComponent->SetMaterial(0, MI_LaserSight);
	LaserMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserMeshComponent->SetVisibility(false);
	LaserMeshComponent->SetRelativeScale3D(FVector(0.025f, 0.025f, 10.0f));
	LaserMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	LaserMeshComponent->SetRelativeLocation(FVector(0.0f, +5'00.0f, 0.0f));
	LaserMeshComponent->SetupAttachment(LaserIntermediateComponent);
}

void APBPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Create dynamic material for ammo ring
	MI_SoftNeon = UMaterialInstanceDynamic::Create(GunAmmoRingComponent->GetMaterial(0), this);
	GunAmmoRingComponent->SetMaterial(0, MI_SoftNeon);

	// Set up text and ring
	SetAmmo(Ammo);
}

void APBPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APBPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &APBPawn::Fire);
	PlayerInputComponent->BindAction(TEXT("Grip"), EInputEvent::IE_Pressed, this, &APBPawn::ToggleLaserSight);
}

void APBPawn::Fire()
{
	FVector MuzzleLoc = GunMeshComponent->GetSocketLocation(NAME_Muzzle);
	FRotator MuzzleRot = GunMeshComponent->GetSocketRotation(NAME_Muzzle);

	if (Ammo == 0)
	{
		// Click sound
		UGameplayStatics::PlaySoundAtLocation(this, Sound_EmptyClip, MuzzleLoc, 1.0f, FMath::RandRange(0.8f, 1.0f));
		
		return;
	}

	// Get player controller & world
	APlayerController* PC = Cast<APlayerController>(GetController());
	GOOSE_BAIL(PC);
	GOOSE_BAIL(PC->PlayerCameraManager);

	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	// Flash white
	PC->PlayerCameraManager->StartCameraFade(0.8f, 0.0f, 0.2f, FLinearColor::White);

	// Expend ammo
	if (Ammo != -1)
	{
		SetAmmo(Ammo - 1);
	}

	// Do a line-trace to an object and cause damage to it
	FVector GunFoward = MuzzleRot.RotateVector(FVector::ForwardVector);

	FCollisionShape Shape;
	Shape.SetSphere(20.0f);
	
	TArray<FHitResult> Hits;
	World->SweepMultiByChannel(Hits, MuzzleLoc, MuzzleLoc + GunFoward * 1'000'00.0f, FQuat::Identity, ECollisionChannel::ECC_Visibility, Shape);

	for (const FHitResult& Hit : Hits)
	{
		if (Hit.Actor.IsValid())
		{
			FDamageEvent DamageEvent;
			Hit.Actor->TakeDamage(1.0f, DamageEvent, PC, this);
		}
	}	

	// Play sound
	UGameplayStatics::PlaySoundAtLocation(this, Sound_Shoot, MuzzleLoc, 1.0f, FMath::RandRange(0.8f, 1.0f));

	// Play haptic effect
	PC->PlayHapticEffect(Haptic_FireGun, EControllerHand::Right, 1.0f);

	// Add tracer
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(MuzzleLoc);
	SpawnTransform.SetRotation(MuzzleRot.Quaternion());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<ATracer>(ATracer::StaticClass(), SpawnTransform, SpawnParams);
}

void APBPawn::ToggleLaserSight()
{
	bool bLaserSightOn = !GetLaserSightOn();
	SetLaserSightOn(bLaserSightOn);

	UWorld* World = GetWorld();
	GOOSE_BAIL(World);

	APlayerController* PC = World->GetFirstPlayerController();
	GOOSE_BAIL(PC);

	// Play sound
	UGameplayStatics::PlaySoundAtLocation(this, bLaserSightOn ? Sound_LaserSightOn : Sound_LaserSightOff, GunMeshComponent->GetComponentLocation(), 1.0f, FMath::RandRange(0.8f, 1.0f));

	// Play haptic effect
	PC->PlayHapticEffect(Haptic_LaserSightToggle, EControllerHand::Right, 1.0f);
}

void APBPawn::SetAmmo(int32 InAmmoAmount)
{
	Ammo = InAmmoAmount;
	
	if (Ammo == Ammo_Infinite) 
	{
		GunAmmoTextComponent->SetVisibility(false);
		GunAmmoRingComponent->SetVisibility(false);
	}
	else
	{
		GunAmmoTextComponent->SetText(FText::FromString(FString::FromInt(Ammo)));
		GunAmmoTextComponent->SetRelativeRotation(FRotator(60.0f, -180.0f, 0.0f));

		FName NAME_Progress = TEXT("Progress");

		GunAmmoTextComponent->SetVisibility(true);
		GunAmmoRingComponent->SetVisibility(true);
		MI_SoftNeon->SetScalarParameterValue(NAME_Progress, float(Ammo) / 100.0f);
	}
}

void APBPawn::SetLaserSightOn(bool bInLaserSightOn)
{
	LaserMeshComponent->SetVisibility(bInLaserSightOn);
}

bool APBPawn::GetLaserSightOn() const
{
	return LaserMeshComponent->IsVisible();
}