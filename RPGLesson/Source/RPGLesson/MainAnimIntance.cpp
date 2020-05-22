// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimIntance.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMainAnimIntance:: NativeInitializeAnimation()
{
    // good practice to check, do we possess a pawn member. 
    if(Pawn==nullptr)
    {
        Pawn=TryGetPawnOwner();
    }
}

void UMainAnimIntance::UpdateAnimationProperties()
{
    // check for our Pawn
    if(Pawn==nullptr)
    {
        Pawn=TryGetPawnOwner();
    }

    
    if (Pawn)
    {
        const FVector Speed = Pawn->GetVelocity(); // getting the velocity (speed) from our Pawn
        const FVector LateralSpeed = FVector(Speed.X,Speed.Y,0.f);  
        MovementSpeed = LateralSpeed.Size(); // we will use it in BP as a main speed parameter for the Character. 

        IsInAir = Pawn->GetMovementComponent()->IsFalling(); // getting from the MovementComponent IsFalling state function
    }
}


