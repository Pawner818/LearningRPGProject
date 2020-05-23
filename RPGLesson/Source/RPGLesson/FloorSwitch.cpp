// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include  "Components/BoxComponent.h"
#include  "Components/StaticMeshComponent.h"
#include  "TimerManager.h"

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

	SwitchTime = 2.f; // 2 sec. delay (used in the TimeHandler)
	bCharacterOnSwitch = false; // is our Character standing on switch plate?

}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	// Add overlap dynamic to interact with the Character
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);

	// Getting current location
	InitialDoorLocation = Door->GetComponentLocation();
	InitialSwitchLocation = FloorTrigger->GetComponentLocation();
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/* When we overlap with the object - in our case it is a door, we call 2 functions - RaiseDoor and LowerDoorSwitch. Both of them implemented in the BP Editor. */
void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if(!bCharacterOnSwitch) bCharacterOnSwitch = true;
	RaiseDoor();
	LowerDoorSwitch();
}

/* When we not overlaping with the trigger, those functions are called  */
void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(bCharacterOnSwitch) bCharacterOnSwitch = false;
	GetWorldTimerManager().SetTimer(SwitchHandle,this,&AFloorSwitch::CloseDoor,SwitchTime);
	
}

void AFloorSwitch::UpdateFloorLocation(float Z)
{
	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z+=Z;
	Door->SetWorldLocation(NewLocation);
}

void AFloorSwitch::UpdateTriggerLocation(float Z)
{
	FVector NewLocation = InitialSwitchLocation;
	NewLocation.Z+=Z;
	TriggerBox->SetWorldLocation(NewLocation);
}

void AFloorSwitch::CloseDoor()
{
	if(!bCharacterOnSwitch)
	{
		LowerDoor();

		RaiseDoorSwitch();
	}
}
