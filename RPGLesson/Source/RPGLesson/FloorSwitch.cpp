// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include  "Components/BoxComponent.h"
#include  "Components/StaticMeshComponent.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(FName("TriggerBox"));
    RootComponent = TriggerBox; // changing the RootComponent of our Actor

	/* enable the proper type of collision we need */
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    /* object type */
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	/* for excluding wrong behaviour, firstly we have to ignore ALL Channels, and then choose the necessary one*/
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	/* we gonna interact with a Pawn type, so we add ECC::Pawn and add the method what we gonna do with it (in our case - ECR_Overlap) */
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);

	
	FloorTrigger = CreateDefaultSubobject<UStaticMeshComponent>(FName("FloorTrigger"));
	FloorTrigger->SetupAttachment(GetRootComponent());

	Door = CreateDefaultSubobject<UStaticMeshComponent>(FName("Door"));
	Door->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);
	
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Warning,TEXT("OnOverlapBegin works!"));
}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp,Warning,TEXT("OnOverlapEnd works!"));
}

