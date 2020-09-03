// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class RPGLESSON_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	public:

	
	/* TODO: Behavior of the enemies using BT && BH 
	 *
	UPROPERTY()
	class UBehaviorTree*EnemyBehaviorTree;
	
	UPROPERTY()
	class UBrainComponent*EnemyBrainComponent;
	
	UPROPERTY()
	class UBlackBoardComponent*EnemyBlackBoardComponent;*/

	
	protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
};
