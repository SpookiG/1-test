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


	// Locking rotation is for some reason based on absolute coordinates so this needs completely redoing



	// Create car wrapper component
	CarWrapper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarWrapper"));
	CarWrapper->SetStaticMesh(ConstructorStatics.WrapperMesh.Get());					// not using this mesh but physics can't be enabled without a mesh
	CarWrapper->SetVisibility(false);
	CarWrapper->SetSimulatePhysics(true);
	CarWrapper->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CarWrapper->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);	// collision enabled to allow physics but we're ignoring all collisions lol, what a mess
	CarWrapper->SetCenterOfMass(FVector(0, 0, 12.5));
	CarWrapper->SetUseCCD(true);
	CarWrapper->BodyInstance.bLockYRotation = true;
	CarWrapper->SetPhysMaterialOverride((UPhysicalMaterial*) ConstructorStatics.SlippyPhysicsMaterial.Get());
	RootComponent = CarWrapper;
	
	// set up car body
	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarBody"));
	CarMesh->SetupAttachment(RootComponent);
	CarMesh->SetStaticMesh(ConstructorStatics.CarMesh.Get());	// Set static mesh
	CarMesh->SetSimulatePhysics(true);
	CarMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CarMesh->SetRelativeRotation(FRotator(270.f, 0.f, 0.f));
	CarMesh->SetRelativeScale3D(FVector(0.25f, 1.f, 2.7f));
	CarMesh->SetCenterOfMass(FVector(0, 0, 12.5));
	CarMesh->SetPhysMaterialOverride((UPhysicalMaterial*) ConstructorStatics.SlippyPhysicsMaterial.Get());
	CarMesh->SetUseCCD(true);

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
	BackThruster = CreateDefaultSubobject<UThruster>(TEXT("BackThruster"));
	BackThruster->SetupAttachment(RootComponent);
	BackThruster->SetupPhysicsConstraint(this);
	BackThruster->SetRelativeLocation(FVector(-10.f, 0.f, -12.5f));
	BackThruster->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	BackThruster->SetHoverForce(50000.f);
	BackThruster->SetThrustForce(300000.f);

	LeftThruster = CreateDefaultSubobject<UThruster>(TEXT("LeftThruster"));
	LeftThruster->SetupAttachment(RootComponent);
	LeftThruster->SetupPhysicsConstraint(this);
	LeftThruster->SetRelativeLocation(FVector(260.f, -100.f, -12.5f));
	LeftThruster->SetRelativeRotation(FRotator(0.f, -90.f, 0.01f));

	RightThruster = CreateDefaultSubobject<UThruster>(TEXT("RightThruster"));
	RightThruster->SetupAttachment(RootComponent);
	RightThruster->SetupPhysicsConstraint(this);
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



	collisions = 0;


	lastTickDelta = 0.f;

	doRespawn = false;
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

	UThruster* lefty;
	UThruster* righty;

	if ((GetActorUpVector() + Cam->GetUpVector()).Size() > 1.f) {
		//UE_LOG(LogTemp, Warning, TEXT("Left"));
		lefty = LeftThruster;
		righty = RightThruster;
	}
	else /*if ((GetActorUpVector() + Cam->GetUpVector()).Size() < .5f)*/ {
		//UE_LOG(LogTemp, Warning, TEXT("Right"));
		lefty = RightThruster;
		righty = LeftThruster;
	}
	//else {
	//	return;
	//}

	if ((GetActorUpVector() + Cam->GetUpVector()).Size() > 0.5f && (GetActorUpVector() + Cam->GetUpVector()).Size() < 1.5f) {
		FVector frontOfShip = (LeftThruster->GetComponentLocation() + RightThruster->GetComponentLocation()) * 0.5f;

		FVector leftDir = FVector::CrossProduct(GetActorForwardVector(), FVector(0, 0, 1));

		if (Val < -.5f) {
			CarWrapper->AddForceAtLocation(leftDir * 40000.0f, frontOfShip);
		}
		else if (Val > .5f) {
			CarWrapper->AddForceAtLocation(-leftDir * 40000.0f, frontOfShip);
		}

		//lefty->SwitchedOn = false;
		//righty->SwitchedOn = false;

		//return;
	}

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


float AFloatyCar::GetGroundCheck() {

	FHitResult hitFloor(ForceInit);
	FVector start = CarMesh->GetComponentLocation();
	FVector end = start - FVector(0, 0, 200.f);
	DrawDebugLine(GetWorld(), start, end, FColor::Green, false, .1f);
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

FVector AFloatyCar::GetAngularVelocity() {
	return CarWrapper->GetPhysicsAngularVelocity(); // TODO when I have the sound
}





// Called when the game starts or when spawned
void AFloatyCar::BeginPlay()
{
	Super::BeginPlay();

	collisions = 0;
	lastVelocity = FVector::ZeroVector;

	respawnPoint = GetActorLocation();
	respawnRotation = GetActorRotation();
}

// Called every frame
void AFloatyCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (doRespawn) {
		CarWrapper->SetSimulatePhysics(true);
		CarMesh->SetSimulatePhysics(true);


		doRespawn = false;
	}

	//if ((GetVelocity() * GetActorUpVector()).Size() > 900.f) {
	//	UE_LOG(LogTemp, Warning, TEXT("full body upwards velocity %f"), (GetVelocity() * GetActorUpVector()).Size());
	//}

	//UE_LOG(LogTemp, Warning, TEXT("space between front thrusters: %f"), (LeftThruster->GetComponentLocation() - RightThruster->GetComponentLocation()).Size());

	FVector frontOfShip = ((LeftThruster->GetComponentLocation() + RightThruster->GetComponentLocation()) * 0.5f) + (LeftThruster->GetUpVector() * 12.5);

	FHitResult hitFloor(ForceInit);
	FVector start = frontOfShip;
	FVector end = start - FVector(0, 0, 200.f/*RightThruster->GetRelativeLocation().Y*/);
	//DrawDebugLine(GetWorld(), start, end, FColor::Green, false, .1f);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	GetWorld()->LineTraceSingleByChannel(hitFloor, start, end, ECC_Visibility, RV_TraceParams);



	// If hit, apply force underneath the thruster
	if (hitFloor.bBlockingHit) {
		//UE_LOG(LogTemp, Warning, TEXT("hit!!"));
		//SetActorLocation(hitCollision.Location);
		//OverlappedComp->ComponentVelocity = -OverlappedComp->GetComponentVelocity();

		groundCheck = 1.f - (hitFloor.Distance / 200.f);

		CarWrapper->SetAngularDamping(100.f);
	}
	else {
		groundCheck = 0.f;

		//UE_LOG(LogTemp, Warning, TEXT("No hit!!"));
		CarWrapper->SetAngularDamping(0.f);
	}

	



	//UE_LOG(LogTemp, Warning, TEXT("Going down! Down speed: %f"), BackThruster->GetComponentLocation().Z - CarWrapper->GetComponentLocation().Z);

	if (BackThruster->GetComponentLocation().Z < LeftThruster->GetComponentLocation().Z + 10.f) {
		
	}




	lastTickDelta = DeltaTime;
	lastVelocity = CarWrapper->GetPhysicsLinearVelocity();
}


void AFloatyCar::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	UE_LOG(LogTemp, Warning, TEXT("Hit checkpoint (car side)!!"));
	


	
}

/*void AFloatyCar::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	
	
}*/




