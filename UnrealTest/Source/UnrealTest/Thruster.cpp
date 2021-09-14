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
	ThrusterMesh->SetSimulatePhysics(true);
	ThrusterMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ThrusterMesh->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	ThrusterMesh->SetCenterOfMass(FVector(0, 0, 12.5));
	ThrusterMesh->SetUseCCD(true);
	ThrusterMesh->SetPhysMaterialOverride((UPhysicalMaterial*) ConstructorStatics.SlippyPhysicsMaterial.Get());


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


	HoverHeight = 100.f;
	HoverForce = 40000.f;						// front thrusters lift at 20000.0, back thrusters at 130000.0
	DampingMultiplier = 0.5f;
	HoverExponent = .1f;

	ThrustForce = 100000.f;


	SwitchedOn = false;


	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	


	UE_LOG(LogTemp, Warning, TEXT("%i"), (uint8) PrimaryComponentTick.TickGroup);
	UE_LOG(LogTemp, Warning, TEXT("Hello"));
	PrimaryComponentTick.TickGroup = TG_DuringPhysics;
	UE_LOG(LogTemp, Warning, TEXT("%i"), (uint8) PrimaryComponentTick.TickGroup);
	UE_LOG(LogTemp, Warning, TEXT("Bello\n"));

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
	lastRelPos = FVector::ZeroVector;
	lastVelocity = FVector::ZeroVector;
	frame = 0;

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
		//FHitResult hitBelowNor(ForceInit);
		FVector nextEnd = start - (HoverHeight * hitBelow.Normal);
		//DrawDebugLine(GetWorld(), start, nextEnd, FColor::Green, false, 10.f);
		//GetWorld()->LineTraceSingleByChannel(hitBelow, start, nextEnd, ECC_Visibility, RV_TraceParams);

		if (hitBelow.bBlockingHit) {

			float distToGround = (hitBelow.Distance / HoverHeight);
			FVector normVel = ThrusterMesh->GetPhysicsLinearVelocity();
			normVel.Normalize();

			// when travelling towards ground limit downward velocity
			if ((normVel + GetUpVector()).Size() < 1.f) {
				FVector upVelocity = ThrusterMesh->GetPhysicsLinearVelocity() * GetUpVector();

				//if (hitBelow.Distance < 30.f) {
				//	upVelocity = -upVelocity;
				//}

				// only damp upwards movement
				//ThrusterMesh->SetPhysicsLinearVelocity((ThrusterMesh->GetPhysicsLinearVelocity() * GetForwardVector()) + (ThrusterMesh->GetPhysicsLinearVelocity() * GetRightVector()) + (upVelocity * DampingMultiplier));



			}


			ThrusterMesh->AddForce(GetUpVector() * HoverForce * powf(1.f - (distToGround), HoverExponent));

			//if (distToGround > .5f) {
			//	ThrusterMesh->AddForce(-GetUpVector() * HoverForce);
			//}
		}
		//if (hitBelow.Distance < 50.f) {
		//	ThrusterMesh->AddForce(GetUpVector() * HoverForce);
		//}

		
	}

	// Now check above the thruster
	FHitResult hitAbove(ForceInit);
	end = start + (HoverHeight * GetUpVector());
	//DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 10.f);

	GetWorld()->LineTraceSingleByChannel(hitAbove, start, end, ECC_Visibility, RV_TraceParams);

	// If hit, apply force above the thruster
	if (hitAbove.bBlockingHit) {
		FVector nextEnd = start - (HoverHeight * hitAbove.Normal);
		//DrawDebugLine(GetWorld(), start, nextEnd, FColor::Red, false, 10.f);
		//GetWorld()->LineTraceSingleByChannel(hitAbove, start, nextEnd, ECC_Visibility, RV_TraceParams);

		if (hitAbove.bBlockingHit) {



			float distToGround = (hitAbove.Distance / HoverHeight);
			FVector normVel = ThrusterMesh->GetPhysicsLinearVelocity();
			normVel.Normalize();

			// when travelling towards ground limit downward velocity
			if ((normVel - GetUpVector()).Size() < 1.f) {
				FVector upVelocity = ThrusterMesh->GetPhysicsLinearVelocity() * GetUpVector();

				//if (hitBelow.Distance < 50.f) {
				//	upVelocity = -upVelocity * (1 / DampingMultiplier);// *.8f;
				//}

				// only damp upwards movement
				//ThrusterMesh->SetPhysicsLinearVelocity((ThrusterMesh->GetPhysicsLinearVelocity() * GetForwardVector()) + (ThrusterMesh->GetPhysicsLinearVelocity() * GetRightVector()) + (upVelocity * DampingMultiplier));
				
			}


			ThrusterMesh->AddForce(-GetUpVector() * HoverForce * powf(1.f - (distToGround), HoverExponent));

			//if (distToGround > .5f) {
			//	ThrusterMesh->AddForce(GetUpVector() * HoverForce);
			//}
		}
	}


	// issue here, possibly because of the delay between hitting thrust and it getting applied
	if (SwitchedOn) {
		FVector thrustDir = -(GetForwardVector() * FVector(1, 1, 0));
		thrustDir.Normalize();

		ThrusterMesh->AddForce(-GetForwardVector() * ThrustForce);
	}


	//if ((ThrusterMesh->GetPhysicsLinearVelocity()).Size() > 2500.f) {
		//UE_LOG(LogTemp, Warning, TEXT("velocity %f"), (ThrusterMesh->GetPhysicsLinearVelocity()).Size());
	//}

	FVector relPos = ThrusterMesh->GetComponentLocation();
	FVector posDiff = relPos - lastRelPos;

	frame++;

	if (posDiff.Size() > 70.f) {
		//UE_LOG(LogTemp, Warning, TEXT("Relative thruster pos difference %f    @    frame %i"), posDiff.Size(), frame);

		//ThrusterMesh->SetWorldLocation(GetComponentLocation());
		//ThrusterMesh->SetPhysicsLinearVelocity(lastVelocity);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Absolute thruster pos difference %f"), (posDiff.Size() * GetRightVector()).Size());

	lastRelPos = relPos;
	lastVelocity = ThrusterMesh->GetPhysicsLinearVelocity();
	// ads

	// I want to raycast up and down to apply a hovering force (even when the car is flipped)
	// I want to check if the car is flipped and reverse the left and right thrusters if so (This is something for the charactercontroller)
	// If thrust is on I want to apply the thrust from the thruster (using forward vector if there is one)

	//ThrusterMesh->AddForce(FVector(50.f, 50.f, 50.f));

	// ...



	/*
	FHitResult hitCollision(ForceInit);
	end = start + (ThrusterMesh->GetPhysicsLinearVelocity() * DeltaTime);
	DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, .1f);

	GetWorld()->LineTraceSingleByChannel(hitCollision, start, end, ECC_Visibility, RV_TraceParams);

	if (hitCollision.bBlockingHit)
	{
		//ThrusterMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		ThrusterMesh->SetPhysicsLinearVelocity(-ThrusterMesh->GetPhysicsLinearVelocity());
	}*/
}

