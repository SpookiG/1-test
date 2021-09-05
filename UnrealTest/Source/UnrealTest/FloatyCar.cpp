// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatyCar.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> CarMesh;
		FConstructorStatics()
			: CarMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cone"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create car wrapper component
	CarWrapper = CreateDefaultSubobject<USceneComponent>(TEXT("CarWrapper"));
	RootComponent = CarWrapper;

	// set up car body
	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarBody"));
	CarMesh->SetupAttachment(RootComponent);
	CarMesh->SetStaticMesh(ConstructorStatics.CarMesh.Get());	// Set static mesh

	// set up thrusters
	BackThrusterSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("BackThrusterSpringArm"));
	BackThrusterSpringArm->SetupAttachment(RootComponent);
	BackThruster = CreateDefaultSubobject<UThruster>(TEXT("BackThruster"));
	BackThruster->SetupAttachment(BackThrusterSpringArm);

	LeftThrusterSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("LeftThrusterSpringArm"));
	LeftThrusterSpringArm->SetupAttachment(RootComponent);
	LeftThruster = CreateDefaultSubobject<UThruster>(TEXT("LeftThruster"));
	LeftThruster->SetupAttachment(LeftThrusterSpringArm);

	RightThrusterSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("RightThrusterSpringArm"));
	RightThrusterSpringArm->SetupAttachment(RootComponent);
	RightThruster = CreateDefaultSubobject<UThruster>(TEXT("RightThruster"));
	RightThruster->SetupAttachment(RightThrusterSpringArm);

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

