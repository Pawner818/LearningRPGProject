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

void AEnemyAIController::MoveToTarget(ARPGLessonCharacter* CharacterToMove)
{
    if(CharacterToMove)
    {
        AAIController*AIController = Cast<AEnemyAIController>(EAIController);
        
        if(EAIController)
        {
            /* Creating a FAIMoveRequest/FNavPathSharePtr to feed them to MoveTo() function */
            FAIMoveRequest MoveRequest;
            MoveRequest.SetGoalActor(CharacterToMove);
            MoveRequest.SetAcceptanceRadius(15.0f);

            FNavPathSharedPtr NavigationPath;

            EAIController->MoveTo(MoveRequest,&NavigationPath);

            // Draw debug sphere which shows the shortest path from Enemy to the main Character
		
            // auto PathPoints = NavigationPath ->GetPathPoints();
            // for (auto Point : PathPoints)
            // {
            // FVector Location = Point.Location;
            // 	UKismetSystemLibrary::DrawDebugSphere(this,Location,25.f,
            //  		8,FLinearColor::Blue,2.5f,1.5f);
            // };
        }
    }
}
		
		

      
    
    






