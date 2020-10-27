// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyPathFollowingComponent.h"
#include "EnemyAIController.generated.h"

/* Forward declaration */
/*class UEnemyPathFollowingComponent;*/
class ARPGLessonCharacter;

/**
 * 
 */
UCLASS()
class RPGLESSON_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	public:
	
	AEnemyAIController(/*const FObjectInitializer& ObjectInitializer*/);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	

	//Ref to the Character
	/*UPROPERTY()
	class ARPGLessonCharacter *MainChar;*/

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	AEnemyAIController*EnemyAIControllerRef;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	AAIController*AIControllerRef;

	virtual void MoveToTheMainCharacter(ARPGLessonCharacter* CharacterToMoveRef);

	
	
	

	

	
	
};
