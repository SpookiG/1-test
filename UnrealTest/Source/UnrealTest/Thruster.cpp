// Fill out your copyright notice in the Description page of Project Settings.


#include "Thruster.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values for this component's properties
UThruster::UThruster()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> ThrusterMesh;
		FConstructorStatics()
			: ThrusterMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cube"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create thruster mesh
	ThrusterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ThrusterMesh"));
	ThrusterMesh->SetupAttachment(this);
	ThrusterMesh->SetStaticMesh(ConstructorStatics.ThrusterMesh.Get());
	ThrusterMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));


	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UThruster::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UThruster::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

