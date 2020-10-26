// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "RPGLessonCharacter.h"
#include "Navigation/PathFollowingComponent.h"

class UEnemyPathFollowingComponent;
class ARPGLessonCharacter;


AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
   : Super(ObjectInitializer.SetDefaultSubobjectClass<UEnemyPathFollowingComponent>(TEXT("PathFollowingComponent")))
{
    
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    
    
    
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
}


