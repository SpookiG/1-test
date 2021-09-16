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

	HoverSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("HoverSpeaker1"));
	HoverSpeaker->bAutoActivate = true;
	HoverSpeaker->VolumeMultiplier = 1.f;
	HoverSpeaker->SetupAttachment(RootComponent);

	BackThrusterSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("BackThrusterSpeaker1"));
	BackThrusterSpeaker->bAutoActivate = true;
	BackThrusterSpeaker->VolumeMultiplier = 1.f;
	BackThrusterSpeaker->SetupAttachment(RootComponent);

	LeftThrusterSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("LeftThrusterSpeaker1"));
	LeftThrusterSpeaker->bAutoActivate = true;
	LeftThrusterSpeaker->VolumeMultiplier = 1.f;
	LeftThrusterSpeaker->SetupAttachment(RootComponent);

	RightThrusterSpeaker = CreateDefaultSubobject<UAudioComponent>(TEXT("RightThrusterSpeaker1"));
	RightThrusterSpeaker->bAutoActivate = true;
	RightThrusterSpeaker->VolumeMultiplier = 1.f;
	RightThrusterSpeaker->SetupAttachment(RootComponent);

	hoverMaxVolume = 1.f;
	hoverMinVolume = .01f;

	thrusterMaxVolume = .8f;
	thrusterMinVolume = .01f;
	backThrusterFadeInTime = .1f;
	backThrusterFadeOutTime = 1.f;
	sideThrusterFadeInTime = .5f;
	sideThrusterFadeOutTime = 1.f;
	sideThrusterFadeOutDelay = 1.f;

	backThrusterCurrentFadeLength = 0.f;
	backThrusterFadeTimeRemaining = 0.f;
	backThrusterFadingIn = false;

	leftThrusterCurrentFadeLength = 0.f;
	leftThrusterFadeTimeRemaining = 0.f;
	leftThrusterFadingIn = false;

	rightThrusterCurrentFadeLength = 0.f;
	rightThrusterFadeTimeRemaining = 0.f;
	rightThrusterFadingIn = false;
	




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
}

// Called every frame
void ACommittedDJ::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// hover gets louder as closer to ground (too far from the ground it fades away completely)
	float newHoverVolume = FMath::Min(ObserveThis->GetGroundCheck() * 3.5f, hoverMaxVolume);		// need to multiply the ground check because the ship hovers abt .3-.5 off the floor
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
		newBackThrusterVolume *= thrusterMaxVolume;

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
		newLeftThrusterVolume *= thrusterMaxVolume;

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
		newRightThrusterVolume *= thrusterMaxVolume;

		RightThrusterSpeaker->AdjustVolume(0.f, FMath::Max(newRightThrusterVolume, thrusterMinVolume));
	}


	
	
	



	/*if (backThrusterFadeTimeRemaining > 0.f)
	{
		backThrusterFadeTimeRemaining = FMath::Max(backThrusterFadeTimeRemaining - DeltaTime, 0.f);
	}
	else
	{
		if (backThrusterFadedIn) {
			if (!ObserveThis->CheckBackThrusterOn()) {
				BackThrusterSpeaker->FadeOut(backThrusterFadeOutTime, .1f);
				backThrusterFadeTimeRemaining = backThrusterFadeOutTime;
				backThrusterFadedIn = false;
			}
		}
		else {
			
		}
	}*/
	
	//else {
	//	BackThrusterSpeaker->FadeOut(backThrusterFadeOutTime, 0.1f);
	//}

	//FString fadingString = BackThrusterSpeaker->bIsFadingOut ? "true" : "false";

	//UE_LOG(LogTemp, Warning, TEXT("fading out? %d"), BackThrusterSpeaker->bIsFadingOut);
}

//float ACommittedDJ::Fade(float fadeLength, float fadeTimeRemaining) {

//}



