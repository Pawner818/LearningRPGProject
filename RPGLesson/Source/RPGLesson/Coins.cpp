// Fill out your copyright notice in the Description page of Project Settings.


#include "Coins.h"
#include "Item.h"




ACoins::ACoins()
{
	CoinsCount = 1;
}

void ACoins::OnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);
	
	if(!ensure(OtherActor))return;
	if(OtherActor)
	{
		ARPGLessonCharacter*Character = Cast<ARPGLessonCharacter>(OtherActor);

		if(!ensure(Character))return;
		if(Character)
		{
			Character ->IncrementCoins(CoinsCount);
			Character -> PickupLocation.Add(GetActorLocation());

			Destroy();
		}
	}
}


void ACoins::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);
	
}



