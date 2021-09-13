// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FloatyCar.h"
#include "GameFramework/PlayerController.h"
#include "CarPilot.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTEST_API ACarPilot : public APlayerController
{
	GENERATED_BODY()

public:
	ACarPilot();

	// Begin overrides
	virtual void Tick(float DeltaSeconds) override;
	// End overrides

protected:

	// Begin overrides
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	// End overrides

	/** Bound to the thrust axis */
	void CallForwardThrust(float Val);

	/** Bound to the vertical axis */
	void CallLeftThrust(float Val);

	/** Bound to the horizontal axis */
	void CallRightThrust(float Val);

	void CallRespawn();

	void Exit();



public:
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite)
	AFloatyCar* Car1;

private:
	

	//UPROPERTY(Category = Thrusters, EditAnywhere)
	//UThruster* BackThruster;

	//UPROPERTY(Category = Thrusters, EditAnywhere)
	//UThruster* LeftThruster;

	//UPROPERTY(Category = Thrusters, EditAnywhere)
	//UThruster* RightThruster;

	bool flipped;
};
