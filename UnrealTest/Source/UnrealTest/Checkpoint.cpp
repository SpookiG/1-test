// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/SceneComponent.h"

#include "FloatyCar.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> CheckpointMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> CheckpointMaterial;

		FConstructorStatics() :
			CheckpointMesh(TEXT("/Game/StarterContent/Shapes/Shape_WideCapsule")),
			CheckpointMaterial(TEXT("/Game/StarterContent/Materials/M_Glass"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;



	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	CheckpointMesh->SetStaticMesh(ConstructorStatics.CheckpointMesh.Get());
	CheckpointMesh->SetMaterial(0, ConstructorStatics.CheckpointMaterial.Get());
	CheckpointMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CheckpointMesh->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);						// I'm not sure how the overlap and hit functions on the car have been triggering for the last week or so but you actually need to send the overlap function to the overlap trigger in the constructor like this

	RootComponent = CheckpointMesh;

	RespawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RespawnPoint"));
	RespawnPoint->SetupAttachment(RootComponent);

	DirectionCheck = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionCheck"));
	DirectionCheck->SetupAttachment(RespawnPoint);


	Goal = false;
	Laps = 0;
	//check(EnableThese);					unreal docs don't seem to give a way to initialize TArrays? Seems unsafe, maybe it's a container thing?
	//check(DisableThese);


 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void ACheckpoint::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit checkpoint!!"));

	// set ship respawn point
	AFloatyCar* car = Cast<AFloatyCar>(OtherActor);
	if (car) {
		car->SetRespawn(RespawnPoint->GetComponentLocation(), RespawnPoint->GetComponentRotation());
	}



	// Disable this checkpoint
	Disable();
}

void ACheckpoint::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACheckpoint::Enable()
{
	CheckpointMesh->SetGenerateOverlapEvents(true);
	CheckpointMesh->SetHiddenInGame(false);
}

void ACheckpoint::Disable()
{
	CheckpointMesh->SetGenerateOverlapEvents(false);
	CheckpointMesh->SetHiddenInGame(true);
}





// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

