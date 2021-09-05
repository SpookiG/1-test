// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatyCar.h"

// Sets default values
AFloatyCar::AFloatyCar()
{
	// Create car wrapper component
	CarWrapper = CreateDefaultSubobject<USceneComponent>(TEXT("PlaneMesh0"));
	RootComponent = CarWrapper;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFloatyCar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloatyCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFloatyCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

