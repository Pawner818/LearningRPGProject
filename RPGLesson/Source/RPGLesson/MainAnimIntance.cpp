// Fill out your copyright notice in the Description page of Project Settings.

#include "MainAnimIntance.h"
#include "RPGLessonCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainAnimIntance:: NativeInitializeAnimation()
{
    // good practice to check, do we possess a pawn member. 
    if(Pawn==nullptr)
    {
        Pawn = TryGetPawnOwner();
        if(Pawn)
        {
            // If we posses a pawn, we cast our main Character class to get an access inside the Editor.
            Character = Cast<ARPGLessonCharacter>(Pawn);            
        }
    }
}   

void UMainAnimIntance::UpdateAnimationProperties()
{
    // check for our Pawn
    if(Pawn == nullptr)
    {
        Pawn = TryGetPawnOwner();
    }

    if (Pawn)
    {
        // speed calculation
        const FVector Speed = Pawn->GetVelocity(); // getting the velocity (speed) from our Pawn
        const FVector LateralSpeed = FVector(Speed.X,Speed.Y,0.f);  
        MovementSpeed = LateralSpeed.Size(); // we will use it in BP as a main speed parameter for the Character.

        // direction calculation 
        const FRotator ActorRotation = Pawn->GetActorRotation();
        Direction = CalculateDirection(Speed,ActorRotation);

        // UE_LOG(LogTemp,Warning, TEXT("Speed is: %f"), &MovementSpeed);
        
        IsInAir = Pawn->GetMovementComponent()->IsFalling(); // getting from the MovementComponent IsFalling state function
        
        
        if(Character == nullptr)
        {
            Character = Cast<ARPGLessonCharacter>(Pawn); 
        }
    }
}



