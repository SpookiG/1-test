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

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* HoverSpeaker;

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* BackThrusterSpeaker;

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* LeftThrusterSpeaker;

	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* RightThrusterSpeaker;
	
	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* SpinSpeaker;

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

	
	UPROPERTY(Category = HoverSounds, EditAnywhere)
	float hoverMinVolume;

	
	UPROPERTY(Category = ThrusterSounds, EditAnywhere)
	float thrusterMinVolume;
	UPROPERTY(Category = ThrusterSounds, EditAnywhere)
	float backThrusterFadeInTime;
	UPROPERTY(Category = ThrusterSounds, EditAnywhere)
	float backThrusterFadeOutTime;
	UPROPERTY(Category = ThrusterSounds, EditAnywhere)
	float sideThrusterFadeInTime;
	UPROPERTY(Category = ThrusterSounds, EditAnywhere)
	float sideThrusterFadeOutTime;
	UPROPERTY(Category = ThrusterSounds, EditAnywhere)
	float sideThrusterFadeOutDelay;

	UPROPERTY(Category = SpinSounds, EditAnywhere)
	float spinFadeInTime;
	UPROPERTY(Category = SpinSounds, EditAnywhere)
	float minAngularVelocityForSpinSound;


	
	float backThrusterCurrentFadeLength;
	float backThrusterFadeTimeRemaining;
	bool backThrusterFadingIn;

	float leftThrusterCurrentFadeLength;
	float leftThrusterFadeTimeRemaining;
	bool leftThrusterFadingIn;

	float rightThrusterCurrentFadeLength;
	float rightThrusterFadeTimeRemaining;
	bool rightThrusterFadingIn;

	float spinFadeTimeRemaining;
	bool spinFadingIn;
};
