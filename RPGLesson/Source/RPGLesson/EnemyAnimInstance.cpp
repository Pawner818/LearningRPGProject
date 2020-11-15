// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    if(Pawn==nullptr)
    {
        Pawn = TryGetPawnOwner();
        if(Pawn)
        {
            Enemy = Cast<AEnemy>(Pawn);
        }
    }
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
    if(Pawn==nullptr)
    {
        Pawn=TryGetPawnOwner();
        if(Pawn)
        {
            Enemy = Cast<AEnemy>(Pawn);
        }
    }
    
    else // Pawn is Valid
    {       
        // speed calculation
        const FVector Speed = Pawn->GetVelocity(); // getting the velocity (speed) from our Pawn
        const FVector LateralSpeed = FVector(Speed.X,Speed.Y,0.f);  
        MovementSpeed = LateralSpeed.Size(); // we will use it in BP as a main speed parameter for the Enemy.
        
        // direction calculation 
        const FRotator ActorRotation = Pawn->GetActorRotation();
        Direction = CalculateDirection(Speed,ActorRotation);

    }
}
