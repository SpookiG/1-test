// Fill out your copyright notice in the Description page of Project Settings.

#include "Thruster.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

// Sets default values for this component's properties
UThruster::UThruster()
{
	if (!HasAnyFlags(RF_ClassDefaultObject)) {

		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UStaticMesh> ThrusterMesh;
			ConstructorHelpers::FObjectFinderOptional<UObject> SlippyPhysicsMaterial;

			FConstructorStatics() :
				ThrusterMesh(TEXT("/Game/StarterContent/Shapes/Shape_Cube")),
				SlippyPhysicsMaterial(TEXT("/Game/car/Slippery"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;



		// Create thruster physics mesh with custom name
		ThrusterMesh = CreateDefaultSubobject<UStaticMeshComponent>(*(GetName().Append("PhysicsMesh")));
		ThrusterMesh->SetupAttachment(this);
		ThrusterMesh->SetStaticMesh(ConstructorStatics.ThrusterMesh.Get());
		ThrusterMesh->BodyInstance.bSimulatePhysics = true;
		ThrusterMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ThrusterMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
		ThrusterMesh->SetCenterOfMass(FVector(0, 0, 12.5));
		ThrusterMesh->BodyInstance.bUseCCD = true;;
		ThrusterMesh->SetPhysMaterialOverride((UPhysicalMaterial*)ConstructorStatics.SlippyPhysicsMaterial.Get());


		// create and attach the physics constraint using the names
		PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(*(GetName().Append("PhysicsConstraint")));
		PhysicsConstraint->SetupAttachment(this);
		PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
		PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
		PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);
		PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
		PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
		PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Locked, 0.f);
		PhysicsConstraint->SetDisableCollision(true);
		PhysicsConstraint->ConstraintInstance.DisableProjection();

	}

	HoverHeight = 100.f;
	HoverForce = 40000.f;						// front thrusters lift at 20000.0, back thrusters at 130000.0
	DampingMultiplier = 0.5f;
	HoverExponent = .1f;

	ThrustForce = 100000.f;


	SwitchedOn = false;


	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	


	PrimaryComponentTick.TickGroup = TG_DuringPhysics;

	// ...
}


void UThruster::SetupPhysicsConstraint(AActor* Parent)
{
	parentActor = Parent;

	if (!HasAnyFlags(RF_ClassDefaultObject)) {
		PhysicsConstraint->ConstraintActor1 = parentActor;
		//PhysicsConstraint->ComponentName1.ComponentName = *GetName();					If I don't give a name I think it attatches to the root mesh, which is what I want
		PhysicsConstraint->ConstraintActor2 = parentActor;
		PhysicsConstraint->ComponentName2.ComponentName = *GetName().Append("PhysicsMesh");
	}
}


void UThruster::SetHoverForce(float val)
{
	HoverForce = val;
}

void UThruster::SetThrustForce(float val)
{
	ThrustForce = val;
}


// TODO: Might be able to remove respawn point passed in, will check back later
void UThruster::Respawn(FVector respawnTo) {
	ThrusterMesh->SetRelativeLocation(GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);
	ThrusterMesh->SetRelativeRotation(GetComponentRotation(), false, nullptr, ETeleportType::ResetPhysics);

	ThrusterMesh->SetPhysicsAngularVelocity(FVector::ZeroVector);
	ThrusterMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
}




// Called when the game starts
void UThruster::BeginPlay()
{
	Super::BeginPlay();

	SwitchedOn = false;

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

	float relativeTargetHeightFromGround = .5f;				// relative because we convert from hover height to a 0-1 range inside the hit check

	// First check underneath the thruster
	FHitResult hitBelow(ForceInit);
	FVector start = ThrusterMesh->GetComponentLocation() + (GetUpVector() * 12.5f); // have to add half of cube size because the pivot isn't in the centre of the cube for some reason
	FVector end = start - (HoverHeight * GetUpVector());
	
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, parentActor);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	GetWorld()->LineTraceSingleByChannel(hitBelow, start, end, ECC_Visibility, RV_TraceParams);

	// might want to do something like:
	// travelling into ground? Add big velocity multiplier away
	// else no velocity multiplier, want just enough force to barely move up the thruster


	// If hit, apply force underneath the thruster
	if (hitBelow.bBlockingHit) {
		FVector nextEnd = start - (HoverHeight * hitBelow.Normal);

		if (hitBelow.bBlockingHit) {

			float distToGround = (hitBelow.Distance / HoverHeight);
			FVector normVel = ThrusterMesh->GetPhysicsLinearVelocity();
			normVel.Normalize();

			// when travelling towards ground limit downward velocity
			if ((normVel + GetUpVector()).Size() < 1.f) {
				FVector upVelocity = ThrusterMesh->GetPhysicsLinearVelocity() * GetUpVector();
			}


			ThrusterMesh->AddForce(GetUpVector() * HoverForce * powf(1.f - (distToGround), HoverExponent));
		}
		
	}

	// Now check above the thruster
	FHitResult hitAbove(ForceInit);
	end = start + (HoverHeight * GetUpVector());

	GetWorld()->LineTraceSingleByChannel(hitAbove, start, end, ECC_Visibility, RV_TraceParams);

	// If hit, apply force above the thruster
	if (hitAbove.bBlockingHit) {
		FVector nextEnd = start - (HoverHeight * hitAbove.Normal);

		if (hitAbove.bBlockingHit) {
			float distToGround = (hitAbove.Distance / HoverHeight);
			FVector normVel = ThrusterMesh->GetPhysicsLinearVelocity();
			normVel.Normalize();

			// when travelling towards ground limit downward velocity
			if ((normVel - GetUpVector()).Size() < 1.f) {
				FVector upVelocity = ThrusterMesh->GetPhysicsLinearVelocity() * GetUpVector();
			}


			ThrusterMesh->AddForce(-GetUpVector() * HoverForce * powf(1.f - (distToGround), HoverExponent));
		}
	}


	// issue here, possibly because of the delay between hitting thrust and it getting applied
	if (SwitchedOn) {
		FVector thrustDir = -(GetForwardVector() * FVector(1, 1, 0));
		thrustDir.Normalize();

		ThrusterMesh->AddForce(-GetForwardVector() * ThrustForce);
	}
}

