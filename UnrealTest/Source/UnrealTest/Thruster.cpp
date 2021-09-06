// Fill out your copyright notice in the Description page of Project Settings.

#include "Thruster.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
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

	// Create thruster mesh with custom name
	ThrusterMesh = CreateDefaultSubobject<UStaticMeshComponent>(*(GetName().Append("Mesh")));
	ThrusterMesh->SetupAttachment(this);
	ThrusterMesh->SetStaticMesh(ConstructorStatics.ThrusterMesh.Get());
	ThrusterMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));


	// create and attach the physics constraint using the names
	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(*(GetName().Append("PhysicsConstraint")));
	PhysicsConstraint->SetupAttachment(this);
	

	

	
	//PhysicsConstraint->ConstraintActor1 = this;
	//PhysicsConstraint->ComponentName1



	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UThruster::SetupPhysicsConstraint(AActor* ParentActor)
{
	PhysicsConstraint->ConstraintActor1 = ParentActor;
	PhysicsConstraint->ComponentName1.ComponentName = *GetName();
	PhysicsConstraint->ConstraintActor2 = ParentActor;
	PhysicsConstraint->ComponentName2.ComponentName = *(GetName().Append("Mesh"));

	

	
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

	//ThrusterMesh->AddForce(FVector(50.f, 50.f, 50.f));

	// ...
}

