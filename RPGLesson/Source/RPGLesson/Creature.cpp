// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Private/KismetTraceUtils.h"
#include "GameFramework/SpringArmComponent.h"





// Sets default values
ACreature::ACreature()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent=CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName("StaticMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());

	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SkeletalMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SkeletalMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("Spring Arm"));
	SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = 400.f; // distance between Camera - Mesh
    SpringArm->AddLocalRotation(FRotator(-20.f,0.f,0.f)); 
	
    Camera=CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // attach camera to the SA Socket
	Camera->AddRelativeRotation(FRotator(-15.f,0.f,0.f));
	
	CurrentVelocity = FVector(0.f,0.f,0.f); // setting up current velocity 
	MaxSpeed = 100.f; // multiply our moving functions by this value (changeable in BP workspace)
	
}


// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACreature::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"),this,&ACreature::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this,&ACreature::MoveRight);
}

void ACreature::MoveForward(float Value)
{
	CurrentVelocity.X = FMath::Clamp(Value,-1.f,1.f) * MaxSpeed;
}

void ACreature::MoveRight(float Value)
{
	CurrentVelocity.Y = FMath::Clamp(Value,-1.f,1.f) * MaxSpeed;
}

