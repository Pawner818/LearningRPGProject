// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"

AWeapon::AWeapon()
{
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMEshComponent"));
    SkeletalMeshComponent->SetupAttachment(GetRootComponent());

    DamageBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBoxComponent"));
    DamageBoxComponent->SetupAttachment(GetRootComponent());
    
}

void AWeapon::BeginPlay()
{
    DamageBoxComponent->OnComponentBeginOverlap.AddDynamic(this,&AWeapon::OnCombatBegin);
    DamageBoxComponent->OnComponentEndOverlap.AddDynamic(this,&AWeapon::OnCombatEnd);

    
}

void AWeapon::Tick(float DeltaSeconds)
{
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // calling base method
    Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    // calling base method
    Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnCombatBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AWeapon::OnCombatEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
}


