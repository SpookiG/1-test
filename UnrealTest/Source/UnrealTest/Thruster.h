// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Thruster.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALTEST_API UThruster : public USceneComponent
{
	GENERATED_BODY()

	/** Thruster needs a non physics mesh to attach to itself w/ a physics constraint because I need physics to apply forces but physics makes it not attached to the main body */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ThrusterNonPhysicsMesh;

	/** Thruster has a body */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ThrusterPhysicsMesh;

	/** Testing this out, can I attach a physics constraint to connect the Thruster mesh to the spring arm? (bcuz applying physics breaks the attachment currently) */
	UPROPERTY(Category = Constraint, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* PhysicsConstraint;

public:	
	// Sets default values for this component's properties
	UThruster();

	// 
	void SetupPhysicsConstraint(AActor* ParentActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	bool debug;

};
