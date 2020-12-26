// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"


void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if(HUDOverlayAsset)
    {
        HUDOverlay = CreateWidget<UUserWidget>(this,HUDOverlayAsset); // set actual overlay if valid
    }

    HUDOverlay->AddToViewport();
    HUDOverlay->SetVisibility(ESlateVisibility::Visible); // visibility of our widget

    if(WEnemyHealthBar)
    {
        EnemyHealthBar = CreateWidget<UUserWidget>(this,WEnemyHealthBar);
        if(EnemyHealthBar)
        {
            EnemyHealthBar->AddToViewport();
            EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    FVector2D VectorAlignment (0.f,0.f);
    EnemyHealthBar->SetAlignmentInViewport(VectorAlignment);
}

void AMainPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(EnemyHealthBar)
    {
        /* get the world location - in our case the enemy current location and convert the coordinates to 2D dimension */
        FVector2D PositionInViewport;
        ProjectWorldLocationToScreen(EnemyLocation,PositionInViewport);

        /* size of the bar */
        const FVector2D SizeInViewport = FVector2D(200.f,20.f);

        /* setting the position in the viewport */
        EnemyHealthBar ->SetPositionInViewport(PositionInViewport);
        EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
    }
}

void AMainPlayerController::DisplayEnemyHealthBar()
{
    if(EnemyHealthBar)
    {
        bEnemyHealthBarVisible = true;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
    }
}

void AMainPlayerController::RemoveHealthBar()
{
    if(EnemyHealthBar)
    {
        bEnemyHealthBarVisible = false;
        EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
    }
}