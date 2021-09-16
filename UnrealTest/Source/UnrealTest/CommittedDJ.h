// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FloatyCar.h"
#include "GameFramework/Actor.h"
#include "CommittedDJ.generated.h"

UCLASS()
class UNREALTEST_API ACommittedDJ : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* AudioContainer;

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* BackingSpeaker;

	/** Thruster has a body */
	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* HoverSpeaker;

	/** Thruster has a body */
	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* BackThrusterSpeaker;

	/** Thruster has a body */
	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* LeftThrusterSpeaker;

	/** Thruster has a body */
	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* RightThrusterSpeaker;
	
public:	
	// Sets default values for this actor's properties
	ACommittedDJ();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


//private:
	//float Fade(float fadeLength, float fadeTimeRemaining);


private:
	UPROPERTY(Category = Sounds, EditAnywhere)
	AFloatyCar* ObserveThis;

	UPROPERTY(Category = Sounds, EditAnywhere)
	float hoverMaxVolume;
	UPROPERTY(Category = Sounds, EditAnywhere)
	float hoverMinVolume;
	UPROPERTY(Category = Sounds, EditAnywhere)
	float thrusterMaxVolume;
	UPROPERTY(Category = Sounds, EditAnywhere)
	float thrusterMinVolume;

	UPROPERTY(Category = Sounds, EditAnywhere)
	float backThrusterFadeInTime;
	UPROPERTY(Category = Sounds, EditAnywhere)
	float backThrusterFadeOutTime;
	UPROPERTY(Category = Sounds, EditAnywhere)
	float sideThrusterFadeInTime;
	UPROPERTY(Category = Sounds, EditAnywhere)
	float sideThrusterFadeOutTime;
	UPROPERTY(Category = Sounds, EditAnywhere)
	float sideThrusterFadeOutDelay;


	
	float backThrusterCurrentFadeLength;
	float backThrusterFadeTimeRemaining;
	bool backThrusterFadingIn;

	float leftThrusterCurrentFadeLength;
	float leftThrusterFadeTimeRemaining;
	bool leftThrusterFadingIn;

	float rightThrusterCurrentFadeLength;
	float rightThrusterFadeTimeRemaining;
	bool rightThrusterFadingIn;
};
