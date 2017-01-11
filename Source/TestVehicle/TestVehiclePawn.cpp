// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TestVehicle.h"
#include "TestVehiclePawn.h"
#include "TestVehicleWheelFront.h"
#include "TestVehicleWheelRear.h"
#include "TestVehicleHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Vehicles/WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine.h"

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IHeadMountedDisplay.h"
#endif // HMD_MODULE_INCLUDED

const FName ATestVehiclePawn::LookUpBinding("LookUp");
const FName ATestVehiclePawn::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

ATestVehiclePawn::ATestVehiclePawn()
{
	// Car mesh
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicles/VH_Buggy/Mesh/SK_Buggy_Vehicle.SK_Buggy_Vehicle"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	//static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicles/VH_Buggy/Blueprint/VH_BuggyAnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	//static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> vehiclePhysMaterial(TEXT("/Game/Vehicles/VH_Buggy/PhysicMaterials/BuggyMaterial.BuggyMaterial"));
	//GetMesh()->SetPhysMaterialOverride(vehiclePhysMaterial.Object);

	// Simulation
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::Open_4W;

	Vehicle4W->WheelSetups[0].WheelClass = UTestVehicleWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("F_L_wheelJNT");

	Vehicle4W->WheelSetups[1].WheelClass = UTestVehicleWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("F_R_wheelJNT");

	Vehicle4W->WheelSetups[2].WheelClass = UTestVehicleWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("B_L_wheelJNT");

	Vehicle4W->WheelSetups[3].WheelClass = UTestVehicleWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("B_R_wheelJNT");

	Vehicle4W->EngineSetup.MaxRPM = 6000;
	Vehicle4W->EngineSetup.TorqueCurve.EditorCurveData.Keys = {
		FRichCurveKey(0.0f, 2500.0f),
		FRichCurveKey(6000.0f, 2500.0f)
	};

	Vehicle4W->DragCoefficient = 1.0f;

	Vehicle4W->InertiaTensorScale = FVector(3.0f, 3.0f, 3.0f);

	//Weapons

	
	RocketTurret = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("RocketTurret"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RocketTurretMesh(TEXT("StaticMesh'/Game/Projectiles/sci_fi_turret_4.sci_fi_turret_4'"));
	RocketTurret->SetStaticMesh(RocketTurretMesh.Object);
//	RocketTurret->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));
	RocketTurret->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
//	RocketTurret->SetRelativeLocation(FVector(43.0f, 0.0f, 277.0f));
	RocketTurret->SetupAttachment(GetMesh(), "TopWeapon");	
	RocketTurret->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 200.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(GetMesh());
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bEnableCameraRotationLag = false;
	SpringArm->CameraRotationLagSpeed = 7.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	
//	SpringArm->bInheritYaw = false;
	//SpringArm->bUseControllerViewRotation = true;
	//SpringArm->bUsePawnControlRotation = true;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Create In-Car camera component 
	InternalCameraOrigin = FVector(-500.0f, 0.0f, 300.0f);

	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = false;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	//Setup TextRenderMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> TextMaterial(TEXT("Material'/Engine/EngineMaterials/AntiAliasedTextMaterialTranslucent.AntiAliasedTextMaterialTranslucent'"));
	
	UMaterialInterface* Material = TextMaterial.Object;

	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetTextMaterial(Material);
	InCarSpeed->SetRelativeLocation(FVector(70.0f, -75.0f, 99.0f));
	InCarSpeed->SetRelativeRotation(FRotator(18.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());
	InCarSpeed->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetTextMaterial(Material);
	InCarGear->SetRelativeLocation(FVector(66.0f, -9.0f, 95.0f));	
	InCarGear->SetRelativeRotation(FRotator(25.0f, 180.0f,0.0f));
	InCarGear->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
	InCarGear->SetupAttachment(GetMesh());
	
	// Colors for the incar gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bInReverseGear = false;

	lastPosition = GetActorLocation();
	lastRotation = GetActorRotation();

	static ConstructorHelpers::FObjectFinder<UBlueprint> BulletBP(TEXT("Blueprint'/Game/Projectiles/Bullet.Bullet'"));
	BulletClass = BulletBP.Object->GeneratedClass;

	
}

void ATestVehiclePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATestVehiclePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATestVehiclePawn::MoveRight);
	PlayerInputComponent->BindAxis("LookUp");
	PlayerInputComponent->BindAxis("LookRight");

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &ATestVehiclePawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &ATestVehiclePawn::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &ATestVehiclePawn::OnToggleCamera);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATestVehiclePawn::OnResetVR); 

	PlayerInputComponent->BindAction("ResetCar", IE_Pressed, this, &ATestVehiclePawn::OnResetCar);
	PlayerInputComponent->BindAction("FirePrimary", IE_Pressed, this, &ATestVehiclePawn::OnFirePrimary);
}

void ATestVehiclePawn::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void ATestVehiclePawn::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void ATestVehiclePawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ATestVehiclePawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ATestVehiclePawn::OnToggleCamera()
{
	EnableIncarView(!bInCarCameraActive);
}

void ATestVehiclePawn::EnableIncarView(const bool bState, const bool bForce)
{
	if ((bState != bInCarCameraActive) || ( bForce == true ))
	{
		bInCarCameraActive = bState;
		
		if (bState == true)
		{
			OnResetVR();
			Camera->Deactivate();
			InternalCamera->Activate();
		}
		else
		{
			InternalCamera->Deactivate();
			Camera->Activate();
		}
		
		InCarSpeed->SetVisibility(bInCarCameraActive);
		InCarGear->SetVisibility(bInCarCameraActive);
	}
}


void ATestVehiclePawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	
	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Set the string in the incar hud
	SetupInCarHUD();

	bool bHMDActive = false;
#if HMD_MODULE_INCLUDED
	if ((GEngine->HMDDevice.IsValid() == true) && ((GEngine->HMDDevice->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if (bHMDActive == false)
	{
		if (InputComponent) 
		{
			HeadDeltaRotation.Pitch = InputComponent->GetAxisValue(LookUpBinding);
			HeadDeltaRotation.Yaw = InputComponent->GetAxisValue(LookRightBinding);
			HeadDeltaRotation.Roll = 0.0f;

			if (bInCarCameraActive == true)
			{
				FRotator HeadRotation = InternalCamera->RelativeRotation;
				HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
				HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
				InternalCamera->RelativeRotation = HeadRotation;
			}
			else
			{
				SpringArm->RelativeRotation += HeadDeltaRotation;
			RocketTurret->RelativeRotation += FRotator(-HeadDeltaRotation.Pitch, HeadDeltaRotation.Yaw, HeadDeltaRotation.Roll);
			}
		}
	}
}

void ATestVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	bool bEnableInCar = false;
#if HMD_MODULE_INCLUDED
	bEnableInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED
	EnableIncarView(bEnableInCar,true);
}

void ATestVehiclePawn::OnResetVR()
{
#if HMD_MODULE_INCLUDED
	if (GEngine->HMDDevice.IsValid())
	{
		GEngine->HMDDevice->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

void ATestVehiclePawn::OnResetCar()
{
	//TeleportTo(lastPosition, FRotator(0.0f, 0.0f, 0.0f));
	Mesh->SetAllPhysicsPosition(lastPosition);
	Mesh->SetAllPhysicsRotation(lastRotation);
	Mesh->SetAllPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f));
	Mesh->SetAllPhysicsAngularVelocity(FVector(0.0f, 0.0f, 0.0f));
	GetVehicleMovementComponent()->SetTargetGear(0, true);
	GetVehicleMovementComponent()->SetThrottleInput(0.0f);
	GetVehicleMovementComponent()->SetSteeringInput(0.0f);
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ATestVehiclePawn::OnFirePrimary()
{
	FVector location = RocketTurret->GetComponentLocation() + RocketTurret->GetForwardVector() * 50.0f + FVector(0.0f, 0.0f, 100.0f);	
	FRotator rotation = RocketTurret->GetComponentRotation() + FRotator(0.0f, 180.0f, 0.0f);
	
	location = RocketTurret->GetSocketLocation("FireHole");
	rotation = RocketTurret->GetSocketRotation("FireHole");

	GetWorld()->SpawnActor(BulletClass, &location, &rotation);
	UE_LOG(LogTemp, Warning, TEXT("Fire1"));
	
}

void ATestVehiclePawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));
	
	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		int32 Gear = GetVehicleMovement()->GetCurrentGear();
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}	
}

void ATestVehiclePawn::SetupInCarHUD()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr) )
	{
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);
		
		if (bInReverseGear == false)
		{
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}
}

#undef LOCTEXT_NAMESPACE
