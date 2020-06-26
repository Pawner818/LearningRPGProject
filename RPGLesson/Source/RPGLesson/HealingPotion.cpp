// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingPotion.h"

AHealingPotion::AHealingPotion()
{
	BottledHealth = 10.f;
}


void AHealingPotion::OnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);

	if(!ensure(OtherActor))return;
	if(OtherActor)
	{
		ARPGLessonCharacter*Character = Cast<ARPGLessonCharacter>(OtherActor);
		if(!ensure(Character))return;
		if(Character)
		{
			Character->IncrementHealth(BottledHealth);

			Destroy();
		}
	}
}


void AHealingPotion::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);
	UE_LOG(LogTemp, Warning, TEXT("HP POtiON Overlap END"));
}