// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyPathFollowingComponent.h"

#include "EnemyAIController.generated.h"

/* Forward declaration */
class UEnemyPathFollowingComponent;

/**
 * 
 */
UCLASS()
class RPGLESSON_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	public:

	UPROPERTY()
	AEnemyAIController*EAIController = nullptr;

	//Ref to the Character
	UPROPERTY()
	class ARPGLessonCharacter*CharacterToMove;
	
	AEnemyAIController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void MoveToTarget(class ARPGLessonCharacter* Character);

	
	
};
