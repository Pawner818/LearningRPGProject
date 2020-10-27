// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "RPGLessonCharacter.h"
#include "Navigation/PathFollowingComponent.h"

class UEnemyPathFollowingComponent;
class ARPGLessonCharacter;


AEnemyAIController::AEnemyAIController()/*(const FObjectInitializer& ObjectInitializer)
   : Super(ObjectInitializer.SetDefaultSubobjectClass<UEnemyPathFollowingComponent>(TEXT("PathFollowingComponent")))*/
{
  
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    EnemyAIControllerRef = Cast<AEnemyAIController>(GetCharacter()->GetController());
    AIControllerRef = Cast<AAIController>(EnemyAIControllerRef);
}

void AEnemyAIController::Tick(float DeltaSeconds)
{
}

void AEnemyAIController::MoveToTheMainCharacter(ARPGLessonCharacter*CharacterToMoveRef)
{		
    if(EnemyAIControllerRef)
    {
        /* Creating a FAIMoveRequest/FNavPathSharePtr to feed them to MoveTo() function */
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalActor(CharacterToMoveRef);
        MoveRequest.SetAcceptanceRadius(15.0f);
    
        FNavPathSharedPtr NavigationPath;
    
        EnemyAIControllerRef->MoveTo(MoveRequest,&NavigationPath);
    }
}


