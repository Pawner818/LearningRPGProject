// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "RPGLessonCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/Gameplaystatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

AWeapon::AWeapon()
{
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SkeletalMeshComponent->SetupAttachment(GetRootComponent());

    DamageBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBoxComponent"));
    DamageBoxComponent->SetupAttachment(GetRootComponent());
    
    bWeaponParticle = false;

    WeaponState = EWeaponStates::EWS_Pickup;



    
}

void AWeapon::EquipWeapon(ARPGLessonCharacter* Character)
{
    if(Character)
    {
        Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
        Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        Mesh->SetSimulatePhysics(false);

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

void AWeapon::BeginPlay()
{
    DamageBoxComponent->OnComponentBeginOverlap.AddDynamic(this,&AWeapon::OnCombatBegin);
    DamageBoxComponent->OnComponentEndOverlap.AddDynamic(this,&AWeapon::OnCombatEnd);

    if(!bWeaponParticle)
    {
        if (ParticleSystem)
        {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),OverlapParticles,GetActorLocation(), FRotator(0.f),true);
        }
    }
    
}

void AWeapon::Tick(float DeltaSeconds)
{
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
            Character->SetActiveOvelappingItem(nullptr);
        }
    }
}

void AWeapon::OnCombatBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AWeapon::OnCombatEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    
}