// Fill out your copyright notice in the Description page of Project Settings.


#include "CarPilot.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"

#include "FloatyCar.h"


// hmm, ok this is kinda complex, Basically I need to:
/*
	find the Pawn in the scene programatically (https://docs.unrealengine.com/4.27/en-US/API/Runtime/Engine/Kismet/UGameplayStatics/GetPlayerPawn/) index is based on AutoPossesPlayer setting in Pawn settings I'm fairly sure
	access the thrusters on the pawns
	Switch on and off dynamically based on the buttons pressed
	Swap left and right thrusters based on which is more left relative to camera or based on if orientation of ship is pointing up or down

*/

ACarPilot::ACarPilot()
{
	flipped = false;

	//check(GetWorld());


	// not sure I need to do anything here, keeping the code below copied from other objects in case tick doesn't work

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
}

void ACarPilot::BeginPlay()
{
	Super::BeginPlay();

	check(GetWorld());

	Car1 = Cast<AFloatyCar>(UGameplayStatics::GetPlayerPawn(this, 0));
}


void ACarPilot::Tick(float DeltaSeconds)
{
	// Need to check here if flipped or not
}


void ACarPilot::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	InputComponent->BindAxis("Thrust", this, &ACarPilot::CallForwardThrust);
	InputComponent->BindAxis("MoveRight", this, &ACarPilot::CallLeftThrust);
}



void ACarPilot::CallForwardThrust(float Val)
{
	check(Car1);

	Car1->ForwardThrust(Val);
}

void ACarPilot::CallLeftThrust(float Val)
{
	check(Car1);

	Car1->LeftThrust(Val);
}

void ACarPilot::CallRightThrust(float Val)
{
	//check(LeftThruster);
	//check(RightThruster);
}