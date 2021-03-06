// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "FloatyCar.generated.h"

UCLASS()
class UNREALTEST_API AFloatyCar : public APawn
{
	GENERATED_BODY()

	/** Wrapper to contain the car & stuff because the mesh used for the body is going to be distorted so using that could distort space for the thrusters */
	UPROPERTY(Category = Wrapper, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CarWrapper;		// this is a static mesh because unreal only likes physics stuff on physics specific objects, the actual mesh is gonna be invisible this is just a wrapper

	/** Body mesh */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CarMesh;

	/** Physics constraint to attach the car wrapper to the body mesh */
	UPROPERTY(Category = Constraint, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* PhysicsConstraint;

	/** The 3 thrusters, I want these to function independantly so they're gonna be a custom C++ pawn component */
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UThruster* BackThruster;
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UThruster* LeftThruster;
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UThruster* RightThruster;

	/** Spring arms for the camera ofc */
	UPROPERTY(Category = Cam, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* RotationSpringArm;
	UPROPERTY(Category = Cam, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* ExtensionSpringArm;

	/** The camera */
	UPROPERTY(Category = Cam, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Cam;


public:
	// Sets default values for this pawn's properties
	AFloatyCar();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	



	void ForwardThrust(float Val);
	void LeftThrust(float Val);
	//void RightThrust();
	void Respawn();
	

	// checkpoint accessed functions
	void SetRespawn(FVector point, FRotator rotation);

	// CommitedDJ accessed functions
	float GetGroundCheck();
	bool CheckBackThrusterOn();
	bool CheckLeftThrusterOn();
	bool CheckRightThrusterOn();
	FVector GetAngularVelocity();			// using this wasn't consistent enough so I'm using the GetGroundCheck to check when to play the spin audio insted, might be able to remove this function later
	


	// 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	int collisions;

	FVector respawnPoint;
	FRotator respawnRotation;

	float groundCheck;
};
