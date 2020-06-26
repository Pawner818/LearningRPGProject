// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Item.h"


AExplosive::AExplosive()
{
	Damage = 20.f;
}


void AExplosive::OnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);

	if(!ensure(OtherActor))return;
	if(OtherActor)
	{
		ARPGLessonCharacter*Character = Cast<ARPGLessonCharacter>(OtherActor);

		if(!ensure(Character))return;
		if(Character)
		{
			Character ->DecrementHealth(Damage);

			Destroy();
		}
	}
}


void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);

}
;


	