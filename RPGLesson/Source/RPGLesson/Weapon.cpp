// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Kismet/Gameplaystatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "RPGLessonCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy.h"

AWeapon::AWeapon()
{
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SkeletalMeshComponent->SetupAttachment(GetRootComponent());

    DamageBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBoxComponent"));
    DamageBoxComponent->SetupAttachment(GetRootComponent());

    Damage = 100.f;

    bWeaponParticle = false;

    WeaponState = EWeaponStates::EWS_Pickup;
}

void AWeapon::BeginPlay()
{
    DamageBoxComponent->OnComponentBeginOverlap.AddDynamic(this,&AWeapon::OnCombatBegin);
    DamageBoxComponent->OnComponentEndOverlap.AddDynamic(this,&AWeapon::OnCombatEnd);

    DamageBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DamageBoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    DamageBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    DamageBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);

    if(!bWeaponParticle)
    {
        if (ParticleSystem)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticles,GetActorLocation(), FRotator(0.f),true);
        }
    }
}

void AWeapon::EquipWeapon(ARPGLessonCharacter* Character)
{

    SetInstigator(Character->GetController());
    
    if(Character)
    {
        SkeletalMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
        SkeletalMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        SkeletalMeshComponent->SetSimulatePhysics(false);

        const USkeletalMeshSocket*WeaponSocket = Character->GetMesh()->GetSocketByName("WeaponSocket");
        if(WeaponSocket)
        {
            WeaponSocket->AttachActor(this,Character->GetMesh());
            bWeaponParticle = true;
            
            Character->SetEquippedWeapon(this);
            Character->SetActiveOvelappingItem(nullptr);
        }

        // sound when we equip a weapon
        if(OnEquppedSound)
        {
            UGameplayStatics::PlaySound2D(this,OnEquppedSound);
        }

        // deactivate the idle particle system
        if(bWeaponParticle)
        {
            IdleParticleSystemComponent->Deactivate();
        }
    }
}

void AWeapon::Tick(float DeltaSeconds)
{
}

void AWeapon::ActivateCollision()
{
    DamageBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
    DamageBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // calling base method
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

        if((WeaponState==EWeaponStates::EWS_Pickup) && OtherActor)
        {
            ARPGLessonCharacter*Character = Cast<ARPGLessonCharacter>(OtherActor);
            if(Character)
            {
                // Character->bFirstTouchToWeapon = true; TODO: when the Player pickup a weapon from the ground, we play special animation + camera zooming 
                Character->SetActiveOvelappingItem(this);
            }
        }
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    // calling base method
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

        if(OtherActor)
        {
            ARPGLessonCharacter*Character = Cast<ARPGLessonCharacter>(OtherActor);
            if(Character)
            {
                // Character->bFirstTouchToWeapon = false;
                Character->SetActiveOvelappingItem(nullptr);
            }
        }
}

void AWeapon::OnCombatBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(OtherActor)
    {
        AEnemy*Enemy = Cast<AEnemy>(OtherActor);
        if(Enemy)
        {
            /* Create a splash of blood when we attack the Enemy */
            if(Enemy->HitParticles)
            {
               // getting an access to the sword socket to play a particle from 
               const USkeletalMeshSocket*EnemyBlood = SkeletalMeshComponent->GetSocketByName("EnemyBlood");
                
               if(EnemyBlood)
               {
                  const FVector SocketLocation = EnemyBlood->GetSocketLocation(SkeletalMeshComponent);
                  UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Enemy->HitParticles,SocketLocation,FRotator(0.f),false);
               }
            }

            /* Create a hit sound when we attack the Enemy */
            if(Enemy->HitSound)
            {
                UGameplayStatics::PlaySound2D(this,Enemy->HitSound);
            }

            /* Damage */
            if(DamageTypeClass)
            {
                UGameplayStatics::ApplyDamage(Enemy,Damage,WeaponInstigator,this,DamageTypeClass);
            }
        }
    }
}

void AWeapon::OnCombatEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
        
}


