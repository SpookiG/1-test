// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Math/Vector.h"
#include "Thruster.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALTEST_API UThruster : public USceneComponent
{
	GENERATED_BODY()


	/** Thruster has a body */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ThrusterMesh;

	/** Need 2 constraints, one from the body to */
	UPROPERTY(Category = Constraint, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* PhysicsConstraint;

public:	
	// Sets default values for this component's properties
	UThruster();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// have to use this to connect the thruster to the main body
	void SetupPhysicsConstraint(AActor* Parent);

	void SetHoverForce(float val);
	void SetThrustForce(float val);
	void Respawn(FVector respawnTo);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:
	// On/Off switch for the thruster (Was going to be a function but all I'm doing is setting a bool)
	bool SwitchedOn;

private:
	UPROPERTY(Category = Hover, EditAnywhere)
	float HoverHeight;

	UPROPERTY(Category = Hover, EditAnywhere)
	float HoverForce;

	/* Damper used to prevent hover force causing car to bounce all over the place. It's applied when the thruster reaches a certain distance from the ground */
	UPROPERTY(Category = Hover, EditAnywhere)
	float DampingMultiplier;

	/* Exponent used to apply more hover force when thruster is closer to ground */
	UPROPERTY(Category = Hover, EditAnywhere)
	float HoverExponent;


	/* The force of the actual thrust */
	UPROPERTY(Category = Thrust, EditAnywhere)
	float ThrustForce;


	bool debug;
	AActor* parentActor;

	FVector lastRelPos;
	FVector lastVelocity;
	int frame;

};
