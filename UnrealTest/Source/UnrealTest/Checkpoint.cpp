// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

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
	CheckpointMesh->SetRelativeScale3D(FVector(20.f, 20.f, 20.f));
	CheckpointMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CheckpointMesh->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);						// I'm not sure how the overlap and hit functions on the car have been triggering for the last week or so but you actually need to send the overlap function to the overlap trigger in the constructor like this

	RootComponent = CheckpointMesh;

	RespawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RespawnPoint"));
	RespawnPoint->SetupAttachment(RootComponent);

	DirectionCheck = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionCheck"));
	DirectionCheck->SetupAttachment(RespawnPoint);


	EnabledOnStartup = false;
	Goal = false;
	Laps = 0;
	NextLevel = "CityBlock";
	//check(EnableThese);					unreal docs don't seem to give a way to initialize TArrays? Seems unsafe, maybe it's a container thing?
	//check(DisableThese);


 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void ACheckpoint::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit checkpoint!!"));

	
	AFloatyCar* car = Cast<AFloatyCar>(OtherActor);
	if (car) {
		// set car respawn point
		car->SetRespawn(RespawnPoint->GetComponentLocation(), RespawnPoint->GetComponentRotation());

		// Disable this checkpoint
		Disable();

		// check if this is a goal checkpoint
		if (Goal) {
			if (Laps <= 0) {
				// completed all laps, startup next level
				UGameplayStatics::OpenLevel(GetWorld(), NextLevel, false);

				return;
			}
			else {
				Laps--;
			}
		}

		// Enable next checkpoints
		for (auto& checkpointToEnable : EnableThese) {
			checkpointToEnable->Enable();
		}

		// Disable any checkpoints from branching routes
		for (auto& checkpointsToDisable : DisableThese) {
			checkpointsToDisable->Disable();
		}

		
	}
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
	
	if (EnabledOnStartup) {
		Enable();					// technically don't need to call this but will put it here anyway just in case I change things about the default checkpoint in the future
	}
	else {
		Disable();
	}
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

