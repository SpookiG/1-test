// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FloatyCar.generated.h"

UCLASS()
class UNREALTEST_API AFloatyCar : public APawn
{
	GENERATED_BODY()

	/** Wrapper to contain the car & stuff because the mesh used for the body is going to be distorted */
	UPROPERTY(Category = Wrapper, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CarWrapper;		// this is a static mesh because unreal only likes physics stuff on physics specific objects, the actual mesh is gonna be empty this is just a wrapper

	/** Body mesh */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CarMesh;

	/** Physics constraint to attach the car wrapper to the body mesh */
	UPROPERTY(Category = Constraint, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* PhysicsConstraint;

	/** I think I need 3 springarms for the 3 thrusters */
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* BackThrusterSpringArm;
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* LeftThrusterSpringArm;
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* RightThrusterSpringArm;

	/** The 3 thrusters, I want these to function independantly so they're gonna be a custom C++ pawn component */
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UThruster* BackThruster;
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UThruster* LeftThruster;
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UThruster* RightThruster;

	/** Spring arm for the camera ofc */
	UPROPERTY(Category = Cam, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CamSpringArm;

	/** The camera */
	UPROPERTY(Category = Cam, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Cam;


public:
	// Sets default values for this pawn's properties
	AFloatyCar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// 
};
