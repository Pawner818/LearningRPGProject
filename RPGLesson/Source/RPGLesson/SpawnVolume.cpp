// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Creature.h"
// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox=CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));

}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawningBox->GetScaledBoxExtent(); 
	FVector Center = SpawningBox->GetComponentLocation(); // origin

	return UKismetMathLibrary::RandomPointInBoundingBox(Center,Extent); // random point between these 2
}

void ASpawnVolume::SpawnOurPawn_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if(ToSpawn) // if it's valid 
	{
		UWorld*World = GetWorld();
		FActorSpawnParameters SpawnParameters;
		if(World)
		{
			ACreature* CreatureSpawned = World->SpawnActor<ACreature>(ToSpawn,Location,FRotator(0.f),SpawnParameters);
		}
	}
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

