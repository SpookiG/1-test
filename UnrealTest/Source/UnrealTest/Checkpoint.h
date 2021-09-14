// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class UNREALTEST_API ACheckpoint : public AActor
{
	// checkpoint does 3 things
	// disables/enables previous & next checkpoints
	// sets player spawn point
	// if a goal and laps = 0 switch to next level

	GENERATED_BODY()

	/** The mesh for the checkpoint */
	UPROPERTY(Category = Wrapper, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CheckpointMesh;

	/** Respawn point for car after hitting checkpoint */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RespawnPoint;

	/** This is just for double checking the direction the respawn point is facing */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* DirectionCheck;
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// declare overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	void Enable();
	void Disable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(Category = Checkpoint, EditAnywhere)
	bool EnabledOnStartup;
	
	UPROPERTY(Category = Checkpoint, EditAnywhere)
	bool Goal;											// if checkpoint is the goal, we want to change to the next level

	UPROPERTY(Category = Checkpoint, EditAnywhere)
	int Laps;

	UPROPERTY(Category = Checkpoint, EditAnywhere)
	TArray<ACheckpoint*> EnableThese;

	UPROPERTY(Category = Checkpoint, EditAnywhere)
	TArray<ACheckpoint*> DisableThese;

};




// On overlap, checkpoint needs to cast the overlapping object to ship & then reset the position
// It also needs to disable it's own mesh and then enable the mesh of the next checkpoint

// If it's the end goal, I want to put end level text on screen, then start a timer and then flick to the next level
// If I want laps, I have to set it so that the end goal has a laps counter
