// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Creature.h"
#include "Enemy.h"
#include "EnemyAIController.h"

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

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	if(SpawnArray.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0,SpawnArray.Num() - 1);

		return SpawnArray[Selection];
	}

	else return nullptr;
}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if(ToSpawn) // if it's valid 
	{
		UWorld*World = GetWorld();
		FActorSpawnParameters SpawnParameters;
		if(World)
		{
			AActor* Actor = World->SpawnActor<AActor>(ToSpawn,Location,FRotator(0.f),SpawnParameters);

			AEnemy*Enemy = Cast<AEnemy>(Actor);
			if(Enemy)
			{
				/* spawn default AI controller */
				Enemy->SpawnDefaultController();

				AEnemyAIController*EnemyAICont = Cast<AEnemyAIController>(Enemy->GetController());
				if(EnemyAICont)
				{
					Enemy->EnemyAIController=EnemyAICont;
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if(Actor_01 || Actor_02 || Actor_03 || Actor_04)
	{	
		SpawnArray.Add(Actor_01);
		SpawnArray.Add(Actor_02);
		SpawnArray.Add(Actor_03);
		SpawnArray.Add(Actor_04);
	}
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

