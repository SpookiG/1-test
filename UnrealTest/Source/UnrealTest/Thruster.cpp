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
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> ThrusterNonPhysicsMesh;
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> ThrusterPhysicsMesh;

		FConstructorStatics() :
			ThrusterNonPhysicsMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cube")),
			ThrusterPhysicsMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cube"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Setup the non physics mesh w/ custom name
	ThrusterNonPhysicsMesh = CreateDefaultSubobject<UStaticMeshComponent>(*(GetName().Append("NonPhysicsMesh")));
	ThrusterNonPhysicsMesh->SetupAttachment(this);
	ThrusterNonPhysicsMesh->SetStaticMesh(ConstructorStatics.ThrusterNonPhysicsMesh.Get());
	ThrusterNonPhysicsMesh->SetVisibility(false);
	//ThrusterNonPhysicsMesh->SetSimulatePhysics(false);
	ThrusterNonPhysicsMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ThrusterNonPhysicsMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	

	// Create thruster physics mesh with custom name
	ThrusterPhysicsMesh = CreateDefaultSubobject<UStaticMeshComponent>(*(GetName().Append("PhysicsMesh")));
	ThrusterPhysicsMesh->SetupAttachment(ThrusterNonPhysicsMesh);
	ThrusterPhysicsMesh->SetStaticMesh(ConstructorStatics.ThrusterPhysicsMesh.Get());
	ThrusterPhysicsMesh->SetSimulatePhysics(true);
	// ThrusterPhysicsMesh->SetEnableGravity(false);											keep??
	ThrusterPhysicsMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ThrusterPhysicsMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
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
	PhysicsConstraint->ComponentName1.ComponentName = *GetName().Append("NonPhysicsMesh");
	PhysicsConstraint->ConstraintActor2 = ParentActor;
	PhysicsConstraint->ComponentName2.ComponentName = *GetName().Append("PhysicsMesh");

	debug = true;

	
}


// Called when the game starts
void UThruster::BeginPlay()
{
	Super::BeginPlay();

	

	//ThrusterPhysicsMesh->SetupAttachment(ThrusterNonPhysicsMesh);
	//ThrusterPhysicsMesh->SetWorldLocation(ThrusterNonPhysicsMesh->GetComponentTransform().GetLocation());

	// ...
	
}


// Called every frame
void UThruster::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (debug) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ThrusterNonPhysicsMesh->GetComponentTransform().GetLocation().ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s"), *ThrusterNonPhysicsMesh->GetComponentTransform().GetLocation().ToString()));
		ThrusterPhysicsMesh->SetRelativeLocation(ThrusterNonPhysicsMesh->GetComponentTransform().GetLocation());

		debug = false;
	}
	//ThrusterMesh->AddForce(FVector(50.f, 50.f, 50.f));

	// ...
}

