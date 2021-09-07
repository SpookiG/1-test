// Fill out your copyright notice in the Description page of Project Settings.

#include "Thruster.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UThruster::UThruster()
{
	
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> ThrusterMesh;

		FConstructorStatics() :
			ThrusterMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cube"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	

	// Create thruster physics mesh with custom name
	ThrusterMesh = CreateDefaultSubobject<UStaticMeshComponent>(*(GetName().Append("PhysicsMesh")));
	ThrusterMesh->SetupAttachment(this);
	ThrusterMesh->SetStaticMesh(ConstructorStatics.ThrusterMesh.Get());
	ThrusterMesh->SetSimulatePhysics(true);
	ThrusterMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ThrusterMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));


	// create and attach the physics constraint using the names
	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(*(GetName().Append("PhysicsConstraint")));
	PhysicsConstraint->SetupAttachment(this);
	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);
	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 3.f);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Limited, 3.f);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 3.f);
	PhysicsConstraint->SetDisableCollision(true);



	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UThruster::SetupPhysicsConstraint(AActor* Parent)
{
	parentActor = Parent;

	PhysicsConstraint->ConstraintActor1 = parentActor;
	//PhysicsConstraint->ComponentName1.ComponentName = *GetName();					If I don't give a name I think it attatches to the root mesh, which is what I want
	PhysicsConstraint->ConstraintActor2 = parentActor;
	PhysicsConstraint->ComponentName2.ComponentName = *GetName().Append("PhysicsMesh");

	debug = true;
}



// Called when the game starts
void UThruster::BeginPlay()
{
	Super::BeginPlay();

	

	//ThrusterMesh->SetupAttachment(ThrusterNonPhysicsMesh);
	//ThrusterMesh->SetWorldLocation(ThrusterNonPhysicsMesh->GetComponentTransform().GetLocation());

	// ...
	
}


// Called every frame
void UThruster::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	check(GetWorld());
	check(parentActor);


	float targetDistFromGround = .5f;

	FHitResult hit(ForceInit);
	FVector start = GetComponentLocation();
	FVector end = start - (100.f * GetUpVector());
	DrawDebugLine(GetWorld(), start, end, FColor::Green, false, .5f);

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, parentActor);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Visibility, RV_TraceParams);

	if (hit.bBlockingHit) {
		float distToGround = (hit.Distance / 100.f);
		FVector normVel = ThrusterMesh->GetPhysicsLinearVelocity();
		normVel.Normalize();

		if (distToGround > targetDistFromGround) {
			ThrusterMesh->SetPhysicsLinearVelocity(ThrusterMesh->GetPhysicsLinearVelocity() * 0.f);
			//ThrusterMesh->AddForce(GetUpVector() * 50000 * powf(distToGround, 2.f) * -1.f);
		}
		else if (distToGround < targetDistFromGround) {
			ThrusterMesh->AddForce(GetUpVector() * 120000 * powf(1.f - (distToGround), 2.f));
		}

		ThrusterMesh->SetPhysicsLinearVelocity(ThrusterMesh->GetPhysicsLinearVelocity() * 0.f);

		//if ((normVel - GetUpVector()).IsNearlyZero()) {
		//	
		//}
		//else {
			//ThrusterMesh->AddForce(GetUpVector() * 100000);
		//}

		//ComponentVelocity = ComponentVelocity.GetClampedToSize(0,5);
	}


	// I want to raycast up and down to apply a hovering force (even when the car is flipped)
	// I want to check if the car is flipped and reverse the left and right thrusters if so (This is something for the charactercontroller)
	// If thrust is on I want to apply the thrust from the thruster (using forward vector if there is one)

	//ThrusterMesh->AddForce(FVector(50.f, 50.f, 50.f));

	// ...
}

