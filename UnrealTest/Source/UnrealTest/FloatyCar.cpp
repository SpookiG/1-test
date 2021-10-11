// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatyCar.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Math/Vector.h"
#include "DrawDebugHelpers.h"

#include "Thruster.h"

// Sets default values
AFloatyCar::AFloatyCar()
{
	BackThruster = CreateDefaultSubobject<UThruster>(TEXT("BackThruster"));
	LeftThruster = CreateDefaultSubobject<UThruster>(TEXT("LeftThruster"));
	RightThruster = CreateDefaultSubobject<UThruster>(TEXT("RightThruster"));

	if (!HasAnyFlags(RF_ClassDefaultObject)) {

		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UStaticMesh> WrapperMesh;
			ConstructorHelpers::FObjectFinderOptional<UStaticMesh> CarMesh;
			ConstructorHelpers::FObjectFinderOptional<UObject> SlippyPhysicsMaterial;

			FConstructorStatics() :
				WrapperMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cone")),
				CarMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cone")),
				SlippyPhysicsMaterial(TEXT("/Game/car/Slippery"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		AutoPossessPlayer = EAutoReceiveInput::Player0;



		// Create car wrapper component
		CarWrapper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarWrapper"));

		CarWrapper->SetStaticMesh(ConstructorStatics.WrapperMesh.Get());					// not using this mesh but physics can't be enabled without a mesh
		CarWrapper->SetVisibility(false);
		CarWrapper->BodyInstance.bSimulatePhysics = true;
		CarWrapper->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CarWrapper->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);	// collision enabled to allow physics but we're ignoring all collisions lol, what a mess
		CarWrapper->SetCenterOfMass(FVector(0, 0, 12.5));
		CarWrapper->BodyInstance.bUseCCD = true;
		CarWrapper->BodyInstance.bLockYRotation = true;
		CarWrapper->SetPhysMaterialOverride((UPhysicalMaterial*)ConstructorStatics.SlippyPhysicsMaterial.Get());
		RootComponent = CarWrapper;

		// set up car body
		CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarBody"));
		CarMesh->SetupAttachment(RootComponent);
		CarMesh->SetStaticMesh(ConstructorStatics.CarMesh.Get());	// Set static mesh
		CarMesh->BodyInstance.bSimulatePhysics = true;
		CarMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CarMesh->SetRelativeRotation(FRotator(270.f, 0.f, 0.f));
		CarMesh->SetRelativeScale3D(FVector(0.25f, 1.f, 2.7f));
		CarMesh->SetCenterOfMass(FVector(0, 0, 12.5));
		CarMesh->SetPhysMaterialOverride((UPhysicalMaterial*)ConstructorStatics.SlippyPhysicsMaterial.Get());
		CarMesh->BodyInstance.bUseCCD = true;

		// Attach car body to wrapper
		PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("BodyConstraint"));
		PhysicsConstraint->SetupAttachment(RootComponent);
		PhysicsConstraint->ConstraintActor1 = this;
		PhysicsConstraint->ComponentName1.ComponentName = TEXT("CarWrapper");
		PhysicsConstraint->ConstraintActor2 = this;
		PhysicsConstraint->ComponentName2.ComponentName = TEXT("CarBody");
		PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
		PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
		PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);
		PhysicsConstraint->SetDisableCollision(true);

		// set up thrusters
		BackThruster->SetupAttachment(RootComponent);
		BackThruster->SetRelativeLocation(FVector(-10.f, 0.f, -12.5f));
		BackThruster->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
		BackThruster->SetHoverForce(50000.f);
		BackThruster->SetThrustForce(300000.f);

		LeftThruster->SetupAttachment(RootComponent);
		LeftThruster->SetRelativeLocation(FVector(260.f, -100.f, -12.5f));
		LeftThruster->SetRelativeRotation(FRotator(0.f, -90.f, 0.01f));

		RightThruster->SetupAttachment(RootComponent);
		RightThruster->SetRelativeLocation(FVector(260.f, 100.f, -12.5f));
		RightThruster->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

		// set up camera
		RotationSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("RotationSpringArm"));
		RotationSpringArm->SetupAttachment(RootComponent);
		RotationSpringArm->SocketOffset = FVector(0.f, 0.f, 0.f);
		RotationSpringArm->TargetArmLength = 0.f;
		RotationSpringArm->bInheritPitch = false;
		RotationSpringArm->bInheritYaw = true;
		RotationSpringArm->bInheritRoll = false;
		RotationSpringArm->bEnableCameraRotationLag = true;
		RotationSpringArm->CameraRotationLagSpeed = 3.f;

		ExtensionSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ExtensionSpringArm"));
		ExtensionSpringArm->SetupAttachment(RotationSpringArm);
		ExtensionSpringArm->SetRelativeLocation(FVector(-50.f, 0.f, 50.f));
		ExtensionSpringArm->SocketOffset = FVector(0.f, 0.f, 130.f);
		ExtensionSpringArm->TargetArmLength = 500.f;
		ExtensionSpringArm->bInheritPitch = true;
		ExtensionSpringArm->bInheritYaw = true;
		ExtensionSpringArm->bInheritRoll = true;
		ExtensionSpringArm->bEnableCameraLag = true;
		ExtensionSpringArm->CameraLagSpeed = 3.f;
		ExtensionSpringArm->CameraLagMaxDistance = 70.f;


		Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
		Cam->SetupAttachment(ExtensionSpringArm);

	}

	BackThruster->SetupPhysicsConstraint(this);
	LeftThruster->SetupPhysicsConstraint(this);
	RightThruster->SetupPhysicsConstraint(this);


	collisions = 0;

	respawnPoint = FVector::ZeroVector;
	respawnRotation = FRotator::ZeroRotator;

	groundCheck = 0.f;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


void AFloatyCar::ForwardThrust(float Val)
{
	check(BackThruster);

	if (Val > .5f) {
		BackThruster->SwitchedOn = true;
	}
	else {
		BackThruster->SwitchedOn = false;
	}
}

void AFloatyCar::LeftThrust(float Val)
{
	check(LeftThruster);
	check(RightThruster);

	// assign left and right thruster based on if the ship is upside down or not
	UThruster* lefty;
	UThruster* righty;

	if (GetActorUpVector().Z > 0.f) {
		lefty = LeftThruster;
		righty = RightThruster;
	}
	else {
		lefty = RightThruster;
		righty = LeftThruster;
	}

	// if car is angled sideways, add force to the front of the car (because the thrusters aren't facing the right direction)
	if (GetActorUpVector().Z > -0.5f && GetActorUpVector().Z < 0.5f) {
		FVector frontOfShip = (LeftThruster->GetComponentLocation() + RightThruster->GetComponentLocation()) * 0.5f;

		FVector leftDir = FVector::CrossProduct(GetActorForwardVector(), FVector(0, 0, 1));

		if (Val < -.5f) {
			CarWrapper->AddForceAtLocation(leftDir * 40000.0f, frontOfShip);
		}
		else if (Val > .5f) {
			CarWrapper->AddForceAtLocation(-leftDir * 40000.0f, frontOfShip);
		}
	}

	//switch on the correct thruster based on the direction held
	check(lefty);
	check(righty);

	if (Val < -.5f) {
		lefty->SwitchedOn = false;
		righty->SwitchedOn = true;
	}
	else if (Val > .5f) {
		lefty->SwitchedOn = true;
		righty->SwitchedOn = false;
	}
	else {
		lefty->SwitchedOn = false;
		righty->SwitchedOn = false;
	}
}



void AFloatyCar::Respawn() {
	SetActorLocationAndRotation(respawnPoint, respawnRotation, false, nullptr, ETeleportType::ResetPhysics);

	// need to reposition and rotate every thruster attached to the ship as well, or the physics constraints start applying forces
	CarMesh->SetRelativeLocation(respawnPoint, false, nullptr, ETeleportType::ResetPhysics);							// This isn't relative??? What???? (I think it's because it's a physics mesh so it gets treated as a root component. Unreal are really into the design decision that physics objects should never be combined ever)
	CarMesh->SetRelativeRotation(FRotator(270.f, 0.f, 0.f) + respawnRotation, false, nullptr, ETeleportType::ResetPhysics);
	BackThruster->Respawn(respawnPoint + respawnRotation.RotateVector(BackThruster->GetRelativeLocation()));
	LeftThruster->Respawn(respawnPoint + respawnRotation.RotateVector(LeftThruster->GetRelativeLocation()));
	RightThruster->Respawn(respawnPoint + respawnRotation.RotateVector(RightThruster->GetRelativeLocation()));


	CarWrapper->SetPhysicsAngularVelocity(FVector::ZeroVector);
	CarWrapper->SetPhysicsLinearVelocity(FVector::ZeroVector);
	CarMesh->SetPhysicsAngularVelocity(FVector::ZeroVector);
	CarMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
}


void AFloatyCar::SetRespawn(FVector point, FRotator rotation) {
	respawnPoint = point;
	respawnRotation = rotation;
}


// data functions for the reactive music code (CommitedDJ)
float AFloatyCar::GetGroundCheck() {
	// the ground check here is longer than the one actually used in the physics tick below, this is because the music was chopping and changing too quickly with the shorter check
	FHitResult hitFloor(ForceInit);
	FVector start = CarMesh->GetComponentLocation();
	FVector end = start - FVector(0, 0, 200.f);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	GetWorld()->LineTraceSingleByChannel(hitFloor, start, end, ECC_Visibility, RV_TraceParams);

	if (hitFloor.bBlockingHit) {
		groundCheck = 1.f - (hitFloor.Distance / 200.f);
	}
	else {
		groundCheck = 0.f;
	}


	return groundCheck;
}

bool AFloatyCar::CheckBackThrusterOn() {
	return BackThruster->SwitchedOn;
}

bool AFloatyCar::CheckLeftThrusterOn() {
	return LeftThruster->SwitchedOn;
}

bool AFloatyCar::CheckRightThrusterOn() {
	return RightThruster->SwitchedOn;
}

// This wasn't used for dynamic audio in the end. For the intended sound clip, the ground check was used instead as it's more reliable
FVector AFloatyCar::GetAngularVelocity() {
	return CarWrapper->GetPhysicsAngularVelocity(); // TODO when I have the sound
}





// Called when the game starts or when spawned
void AFloatyCar::BeginPlay()
{
	Super::BeginPlay();

	collisions = 0;

	respawnPoint = GetActorLocation();
	respawnRotation = GetActorRotation();
}

// Called every frame
void AFloatyCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	FVector frontOfShip = ((LeftThruster->GetComponentLocation() + RightThruster->GetComponentLocation()) * 0.5f) + (LeftThruster->GetUpVector() * 12.5);

	FHitResult hitFloor(ForceInit);
	FVector start = frontOfShip;
	FVector end = start - FVector(0, 0, 200.f);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	GetWorld()->LineTraceSingleByChannel(hitFloor, start, end, ECC_Visibility, RV_TraceParams);

	// If hit, add angular damping so car has more control on roads etc
	if (hitFloor.bBlockingHit) {

		groundCheck = 1.f - (hitFloor.Distance / 200.f);
		CarWrapper->SetAngularDamping(100.f);
	}
	else {
		groundCheck = 0.f;
		CarWrapper->SetAngularDamping(0.f);
	}
}





