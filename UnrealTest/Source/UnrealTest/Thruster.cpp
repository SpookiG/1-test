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
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> ThrusterPhysicsMesh;
		FConstructorStatics()
			: ThrusterPhysicsMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cube"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create thruster mesh with custom name
	ThrusterPhysicsMesh = CreateDefaultSubobject<UStaticMeshComponent>(*(GetName().Append("Mesh")));
	ThrusterPhysicsMesh->SetupAttachment(this);
	ThrusterPhysicsMesh->SetSimulatePhysics(true);
	// ThrusterPhysicsMesh->SetEnableGravity(false);											keep??
	ThrusterPhysicsMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ThrusterPhysicsMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);


	ThrusterPhysicsMesh->SetStaticMesh(ConstructorStatics.ThrusterPhysicsMesh.Get());
	ThrusterPhysicsMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));


	// create and attach the physics constraint using the names
	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(*(GetName().Append("PhysicsConstraint")));
	PhysicsConstraint->SetupAttachment(this);
	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);
	PhysicsConstraint->SetDisableCollision(true);
	

	

	
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

