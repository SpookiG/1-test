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

	LeftThruster = CreateDefaultSubobject<UThruster>(TEXT("LeftThruster"));
	LeftThruster->SetupAttachment(RootComponent);
	LeftThruster->SetupPhysicsConstraint(this);
	LeftThruster->SetRelativeLocation(FVector(260.f, -30.f, -12.5f));
	LeftThruster->SetRelativeRotation(FRotator(0.f, 90.f, 0.01f));

	RightThruster = CreateDefaultSubobject<UThruster>(TEXT("RightThruster"));
	RightThruster->SetupAttachment(RootComponent);
	RightThruster->SetupPhysicsConstraint(this);
	RightThruster->SetRelativeLocation(FVector(260.f, 30.f, -12.5f));
	RightThruster->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// set up camera
	CamSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CamSpringArm->SetupAttachment(RootComponent);

	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Cam->SetupAttachment(CamSpringArm);







 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFloatyCar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloatyCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFloatyCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

