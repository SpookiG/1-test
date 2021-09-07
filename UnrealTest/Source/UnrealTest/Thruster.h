// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
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
