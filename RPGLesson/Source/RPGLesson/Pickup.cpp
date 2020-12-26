// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"


APickup::APickup()
{
}

void APickup::OnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);
	
	if(!ensure(OtherActor))return;
	if(OtherActor)
	{
		ARPGLessonCharacter*Character = Cast<ARPGLessonCharacter>(OtherActor);
		
		if(Character)
		{
			OnPickupBP(Character);
			Character -> PickupLocation.Add(GetActorLocation());

			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticles,GetActorLocation(), FRotator(0.f),true);
			}
			
			if(SoundCue)
			{
				UGameplayStatics::PlaySound2D(this,SoundCue);
			}

			Destroy();
		}
	}
}


void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);
	
}



