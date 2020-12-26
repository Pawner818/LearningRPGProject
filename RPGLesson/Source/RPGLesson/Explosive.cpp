// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"


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
		AEnemy*Enemy = Cast<AEnemy>(OtherActor); //comment this line if you don't want to explode the Enemy

		if(Character || Enemy)
		{
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticles,GetActorLocation(), FRotator(0.f),true);
			}
			
			if(SoundCue)
			{
				UGameplayStatics::PlaySound2D(this,SoundCue);
			}

			UGameplayStatics::ApplyDamage(OtherActor,Damage,nullptr,this,DamageTypeClass);
			
			Destroy();
		}
	}
}


void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex);

}
;


	