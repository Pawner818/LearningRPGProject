// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "RPGLessonCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
    CollisionVolume=CreateDefaultSubobject<USphereComponent>(FName("CollisionVolume"));
	RootComponent=CollisionVolume;

	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AItem::OnOverlapEnd);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	IdleParticleSystemComponent=CreateDefaultSubobject<UParticleSystemComponent>(FName("IdleParticleSystemComponent"));
	IdleParticleSystemComponent->SetupAttachment(GetRootComponent());
}



// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (OverlapParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticles,GetActorLocation(), FRotator(0.f),true);
	}
	if(SoundCue)
	{
		UGameplayStatics::PlaySound2D(this,SoundCue);
	}
	
	
	
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	
}


