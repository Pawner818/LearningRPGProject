// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class RPGLESSON_API ASpawnVolume : public AActor
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

    // Area where we would like to spawn our Pawn
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Spawning")
	class UBoxComponent*SpawningBox;

    // Random point inside Box, which we use as a point where we can spawn our Pawn.
	UFUNCTION(BlueprintPure,Category="Spawning")
	FVector GetSpawnPoint();

	// Not a reference, because we create a new object of type ACreature. Used in SpawnOurPawn function inside BP Editor.
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Spawning")
	TSubclassOf<class ACreature>PawnToSpawn;

	// Spawning our pawn
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category="Spawning")
	void SpawnOurPawn(UClass*ToSpawn, const FVector&Location);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
