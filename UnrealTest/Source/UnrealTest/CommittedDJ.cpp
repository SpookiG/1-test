// Fill out your copyright notice in the Description page of Project Settings.


#include "CommittedDJ.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "Math/NumericLimits.h"

#include "FloatyCar.h"

// Sets default values
ACommittedDJ::ACommittedDJ()
{
	AudioContainer = CreateDefaultSubobject<USceneComponent>(TEXT("AudioContainer"));
	RootComponent = AudioContainer;

	
	BackingSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("BackingSpeaker"));
	BackingSpeaker->bAutoActivate = true;
	BackingSpeaker->VolumeMultiplier = 1.f;
	BackingSpeaker->VolumeMultiplier = 1.2f;
	BackingSpeaker->SetupAttachment(RootComponent);

	HoverSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("HoverSpeaker"));
	HoverSpeaker->bAutoActivate = true;
	HoverSpeaker->VolumeMultiplier = 1.f;
	HoverSpeaker->VolumeMultiplier = 1.5f;
	HoverSpeaker->SetupAttachment(RootComponent);

	BackThrusterSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("BackThrusterSpeaker"));
	BackThrusterSpeaker->bAutoActivate = true;
	BackThrusterSpeaker->VolumeMultiplier = 1.f;
	BackThrusterSpeaker->VolumeMultiplier = .5f;
	BackThrusterSpeaker->SetupAttachment(RootComponent);

	LeftThrusterSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("LeftThrusterSpeaker"));
	LeftThrusterSpeaker->bAutoActivate = true;
	LeftThrusterSpeaker->VolumeMultiplier = 1.f;
	LeftThrusterSpeaker->VolumeMultiplier = .5f;
	LeftThrusterSpeaker->SetupAttachment(RootComponent);

	RightThrusterSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("RightThrusterSpeaker"));
	RightThrusterSpeaker->bAutoActivate = true;
	RightThrusterSpeaker->VolumeMultiplier = 1.f;
	RightThrusterSpeaker->VolumeMultiplier = .5f;
	RightThrusterSpeaker->SetupAttachment(RootComponent);

	SpinSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("SpinSpeakerr"));
	SpinSpeaker->bAutoActivate = true;
	SpinSpeaker->VolumeMultiplier = 1.f;
	SpinSpeaker->VolumeMultiplier = .9f;
	SpinSpeaker->SetupAttachment(RootComponent);

	hoverMinVolume = .01f;

	thrusterMinVolume = .01f;
	backThrusterFadeInTime = .1f;
	backThrusterFadeOutTime = 1.f;
	sideThrusterFadeInTime = .1f;
	sideThrusterFadeOutTime = 1.f;
	sideThrusterFadeOutDelay = 1.f;

	spinFadeInTime = 2.f;
	minAngularVelocityForSpinSound = 50.f;

	backThrusterCurrentFadeLength = 0.f;
	backThrusterFadeTimeRemaining = 0.f;
	backThrusterFadingIn = false;

	leftThrusterCurrentFadeLength = 0.f;
	leftThrusterFadeTimeRemaining = 0.f;
	leftThrusterFadingIn = false;

	rightThrusterCurrentFadeLength = 0.f;
	rightThrusterFadeTimeRemaining = 0.f;
	rightThrusterFadingIn = false;
	
	spinFadeTimeRemaining = 0.f;
	spinFadingIn = false;



 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACommittedDJ::BeginPlay()
{
	Super::BeginPlay();

	check(ObserveThis);

	HoverSpeaker->AdjustVolume(0.f, hoverMinVolume);
	BackThrusterSpeaker->AdjustVolume(0.f, thrusterMinVolume);
	LeftThrusterSpeaker->AdjustVolume(0.f, thrusterMinVolume);
	RightThrusterSpeaker->AdjustVolume(0.f, thrusterMinVolume);
	SpinSpeaker->AdjustVolume(0.f, thrusterMinVolume);
}

// Called every frame
void ACommittedDJ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// hover gets louder as closer to ground (too far from the ground it fades away completely)
	float newHoverVolume = FMath::Min(ObserveThis->GetGroundCheck() * 3.5f, 1.f);		// need to multiply the ground check because the ship hovers abt .3-.5 off the floor
	HoverSpeaker->AdjustVolume(0.f, FMath::Max(newHoverVolume, hoverMinVolume));								// weird bug, if volume is adjusted to 0 it doesn't come back up again, maybe it's adjusting w/ a multiplier? Funky


	// thrusters fade in and out if they are in use
	// kinda messy code w/ too many flags but I don't wanna make a state machinee
	// can't use unreal's fade in/out functions because they designed them to restart the sound even though there are already functions specifically for restart the sound that I'm not calling here. Thanks for the convinience I guess, I have to make my own fade now
	// start by doing a state check
	if (backThrusterFadingIn) {
		// if we're fading in I want to not allow fading out until fading in is done
		if (backThrusterFadeTimeRemaining <= 0.f) {
			if (!ObserveThis->CheckBackThrusterOn()) {
				backThrusterCurrentFadeLength = backThrusterFadeOutTime;
				backThrusterFadeTimeRemaining = backThrusterFadeOutTime;
				backThrusterFadingIn = false;
			}
		}
	}
	else {
		// if fading out I don't really mind
		if (ObserveThis->CheckBackThrusterOn()) {
			backThrusterCurrentFadeLength = backThrusterFadeInTime;
			backThrusterFadeTimeRemaining = backThrusterFadeInTime;
			backThrusterFadingIn = true;
		}
	}

	// then handle any current fades
	if (backThrusterFadeTimeRemaining > 0.f)
	{
		backThrusterFadeTimeRemaining = FMath::Max(backThrusterFadeTimeRemaining - DeltaTime, 0.f);

		float newBackThrusterVolume = (backThrusterFadeTimeRemaining / backThrusterCurrentFadeLength);
		newBackThrusterVolume = backThrusterFadingIn ? 1.f - newBackThrusterVolume : newBackThrusterVolume;

		BackThrusterSpeaker->AdjustVolume(0.f, FMath::Max(newBackThrusterVolume, thrusterMinVolume));
	}


	// now for the left thruster
	if (leftThrusterFadingIn) {
		// if we're fading in I want to not allow fading out until fading in is done
		if (leftThrusterFadeTimeRemaining <= 0.f) {
			if (!ObserveThis->CheckLeftThrusterOn()) {
				leftThrusterCurrentFadeLength = sideThrusterFadeOutTime;
				leftThrusterFadeTimeRemaining = sideThrusterFadeOutTime + sideThrusterFadeOutDelay;					// added code for side thrusters to handle delay so sounds stay playing at full volume longer
				leftThrusterFadingIn = false;
			}
		}
	}
	else {
		// if fading out I don't really mind
		if (ObserveThis->CheckLeftThrusterOn()) {
			leftThrusterCurrentFadeLength = sideThrusterFadeInTime;
			leftThrusterFadeTimeRemaining = sideThrusterFadeInTime;
			leftThrusterFadingIn = true;
		}
	}

	// then handle any current fades
	if (leftThrusterFadeTimeRemaining > 0.f)
	{
		leftThrusterFadeTimeRemaining = FMath::Max(leftThrusterFadeTimeRemaining - DeltaTime, 0.f);

		float newLeftThrusterVolume = (leftThrusterFadeTimeRemaining / leftThrusterCurrentFadeLength);
		newLeftThrusterVolume = leftThrusterFadingIn ? 1.f - newLeftThrusterVolume : newLeftThrusterVolume;
		newLeftThrusterVolume = FMath::Min(newLeftThrusterVolume, 1.f);												// added code for side thrusters to handle delay so sounds stay playing at full volume longer

		LeftThrusterSpeaker->AdjustVolume(0.f, FMath::Max(newLeftThrusterVolume, thrusterMinVolume));
	}


	// and the right thruster
	if (rightThrusterFadingIn) {
		// if we're fading in I want to not allow fading out until fading in is done
		if (rightThrusterFadeTimeRemaining <= 0.f) {
			if (!ObserveThis->CheckRightThrusterOn()) {
				rightThrusterCurrentFadeLength = sideThrusterFadeOutTime;
				rightThrusterFadeTimeRemaining = sideThrusterFadeOutTime + sideThrusterFadeOutDelay;				// added code for side thrusters to handle delay so sounds stay playing at full volume longer
				rightThrusterFadingIn = false;
			}
		}
	}
	else {
		// if fading out I don't really mind
		if (ObserveThis->CheckRightThrusterOn()) {
			rightThrusterCurrentFadeLength = sideThrusterFadeInTime;
			rightThrusterFadeTimeRemaining = sideThrusterFadeInTime;
			rightThrusterFadingIn = true;
		}
	}

	// then handle any current fades
	if (rightThrusterFadeTimeRemaining > 0.f)
	{
		rightThrusterFadeTimeRemaining = FMath::Max(rightThrusterFadeTimeRemaining - DeltaTime, 0.f);

		float newRightThrusterVolume = (rightThrusterFadeTimeRemaining / rightThrusterCurrentFadeLength);
		newRightThrusterVolume = rightThrusterFadingIn ? 1.f - newRightThrusterVolume : newRightThrusterVolume;
		newRightThrusterVolume = FMath::Min(newRightThrusterVolume, 1.f);											// added code for side thrusters to handle delay so sounds stay playing at full volume longer

		RightThrusterSpeaker->AdjustVolume(0.f, FMath::Max(newRightThrusterVolume, thrusterMinVolume));
	}


	
	// I think for the spin check I want to compare angular velocity to a different value for fading in the spin sound and fading out.
	// I don't want it to start too eagerly if the car is just, like, turing a bit on the floor but also I want to keep the sound going even if the spinning slows down a bit
	// idk, just see how a single variable goes for now I guess, can always add later
	if (ObserveThis->GetGroundCheck() == 0.f) {
		if (!spinFadingIn) {
			spinFadeTimeRemaining = spinFadeInTime;
		}
		spinFadingIn = true;
	}
	else {
		SpinSpeaker->AdjustVolume(0.f, thrusterMinVolume);
		spinFadeTimeRemaining = 0.f;
		spinFadingIn = false;
	}
	
	
	if (spinFadeTimeRemaining > 0.f) {
		spinFadeTimeRemaining = FMath::Max(spinFadeTimeRemaining - DeltaTime, 0.f);

		float newSpinVolume = 1.f - (spinFadeTimeRemaining / spinFadeInTime);

		SpinSpeaker->AdjustVolume(0.f, FMath::Max(newSpinVolume, thrusterMinVolume));
	}
}



