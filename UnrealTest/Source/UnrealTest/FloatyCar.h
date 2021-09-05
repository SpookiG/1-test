// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FloatyCar.generated.h"

UCLASS()
class UNREALTEST_API AFloatyCar : public APawn
{
	GENERATED_BODY()

	/** Wrapper to contain the car & stuff because the mesh used for the body is going to be distorted */
	UPROPERTY(Category = Wrapper, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* CarWrapper;

public:
	// Sets default values for this pawn's properties
	AFloatyCar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// 
};
