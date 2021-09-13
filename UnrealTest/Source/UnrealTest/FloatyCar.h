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

	// declare overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);

	// declare overlap end function
	//UFUNCTION()
	//void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	void ForwardThrust(float Val);
	void LeftThrust(float Val);

	void Respawn();
	//void RightThrust();


	// 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	float lastTickDelta;
	int collisions;
	FVector lastVelocity;

	bool doRespawn;
	FVector respawnPoint;
	FRotator respawnRotation;
};
