// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatyCar.h"
#include "UnrealTestPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Math/Vector.h"

#include "Thruster.h"

// Sets default values
AFloatyCar::AFloatyCar()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> WrapperMesh;
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> CarMesh;

		FConstructorStatics() :
			WrapperMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cone")),
			CarMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cone"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create car wrapper component
	CarWrapper = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarWrapper"));
	CarWrapper->SetStaticMesh(ConstructorStatics.WrapperMesh.Get());					// not using this mesh but physics can't be enabled without a mesh
	CarWrapper->SetVisibility(false);
	CarWrapper->SetSimulatePhysics(true);
	CarWrapper->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CarWrapper->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);	// collision enabled to allow physics but we're ignoring all collisions lol, what a mess
	RootComponent = CarWrapper;
	
	// set up car body
	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarBody"));
	CarMesh->SetupAttachment(RootComponent);
	CarMesh->SetStaticMesh(ConstructorStatics.CarMesh.Get());	// Set static mesh
	CarMesh->SetSimulatePhysics(true);
	CarMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CarMesh->SetRelativeRotation(FRotator(270.f, 0.f, 0.f));
	CarMesh->SetRelativeScale3D(FVector(0.25f, 1.f, 2.7f));

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
	BackThruster->SetHoverForce(130000.f);

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
	CamSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CamSpringArm->SetupAttachment(RootComponent);
	CamSpringArm->SocketOffset = FVector(-150.f, 0.f, 150.f);
	CamSpringArm->bInheritPitch = false;
	CamSpringArm->bInheritRoll = false;
	CamSpringArm->bEnableCameraLag = true;
	CamSpringArm->CameraLagSpeed = 1.f;
	CamSpringArm->bEnableCameraRotationLag = true;
	CamSpringArm->CameraRotationLagSpeed = 1.f;
	CamSpringArm->CameraLagMaxDistance = 70.f;



	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Cam->SetupAttachment(CamSpringArm);



	collisions = 0;


	lastTickDelta = 0.f;

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



// Called when the game starts or when spawned
void AFloatyCar::BeginPlay()
{
	Super::BeginPlay();

	collisions = 0;
	lastVelocity = FVector::ZeroVector;
}

// Called every frame
void AFloatyCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if ((GetVelocity() * GetActorUpVector()).Size() > 900.f) {
	//	UE_LOG(LogTemp, Warning, TEXT("full body upwards velocity %f"), (GetVelocity() * GetActorUpVector()).Size());
	//}

	//UE_LOG(LogTemp, Warning, TEXT("space between front thrusters: %f"), (LeftThruster->GetComponentLocation() - RightThruster->GetComponentLocation()).Size());

	FVector frontOfShip = (LeftThruster->GetComponentLocation() + RightThruster->GetComponentLocation()) * 0.5f;

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
		CarWrapper->SetAngularDamping(100.f);
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("No hit!!"));
		CarWrapper->SetAngularDamping(0.f);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Angular damping: %f"), CarWrapper->GetAngularDamping());

	if (LeftThruster->SwitchedOn) {
		UE_LOG(LogTemp, Warning, TEXT("Left thruster on"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Left thruster off"));
	}

	if (RightThruster->SwitchedOn) {
		UE_LOG(LogTemp, Warning, TEXT("Right thruster on\n"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Right thruster off\n"));
	}



	//UE_LOG(LogTemp, Warning, TEXT("Going down! Down speed: %f"), BackThruster->GetComponentLocation().Z - CarWrapper->GetComponentLocation().Z);

	if (BackThruster->GetComponentLocation().Z < LeftThruster->GetComponentLocation().Z + 10.f) {
		
	}




	lastTickDelta = DeltaTime;
	lastVelocity = CarWrapper->GetPhysicsLinearVelocity();
}


void AFloatyCar::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// First check underneath the thruster
	//FHitResult hitCollision(ForceInit);
	//FVector start = OverlappedComp->GetComponentLocation();
	//FVector end = start - (OverlappedComp->GetComponentVelocity() * lastTickDelta * 2.f);
	//DrawDebugLine(GetWorld(), start, end, FColor::Green, false, .1f);
	//FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	//RV_TraceParams.bTraceComplex = true;
	//RV_TraceParams.bReturnPhysicalMaterial = false;

	//GetWorld()->LineTraceSingleByChannel(hitCollision, start, end, ECC_Visibility, RV_TraceParams);



	// If hit, apply force underneath the thruster
	//if (hitCollision.bBlockingHit) {
		//SetActorLocation(hitCollision.Location);
		//OverlappedComp->ComponentVelocity = -OverlappedComp->GetComponentVelocity();
	//}


	
}

void AFloatyCar::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Other != this && NormalImpulse.Size() > 10000.f)
	{
		//collisions++;
		//UE_LOG(LogTemp, Warning, TEXT("collision %i!! Impluse = %f   problem check = %f"), collisions, NormalImpulse.Size(), (MyComp->ComponentVelocity - GetVelocity()).Size());

		// linecast around car to see if we're colliding with anything in front or behind

		if ((HitNormal * GetActorForwardVector()).Size() > 0.8f)
		{
			
			//UE_LOG(LogTemp, Warning, TEXT("problem collision %i"), collisions, NormalImpulse.Size(), (HitNormal * GetVelocity()).Size());
			//CarWrapper->SetPhysicsLinearVelocity(lastVelocity);
			//CarMesh->SetPhysicsLinearVelocity(lastVelocity);
			//CarWrapper->SetPhysicsAngularVelocity(FVector::ZeroVector);
			//CarMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		}
	}
	
}





// Called to bind functionality to input
//void AFloatyCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);

//}

