// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGLessonCharacter.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"



//////////////////////////////////////////////////////////////////////////
// ARPGLessonCharacter - main Character of this RPG-demo scene. 


ARPGLessonCharacter::ARPGLessonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 500.f; // the power of jump force
	GetCharacterMovement()->AirControl = 0.2; // how long the Character will land 

	// Create a camera boom (pulls in towards the player if there is a collision)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Camera lag setup (light smooth effect when you moving around the Character)
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagMaxDistance = 70;
	SpringArm->CameraLagSpeed = 5;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	

    TraceDistance = 100.f; // How far we can reach (used in LineTrace function)
    bHit = false;

	/* TODO: find out why we can't use skeletal mesh to generate overlap event's */
	// Mesh->OnComponentBeginOverlap.AddDynamic(this, &ARPGLessonCharacter::OnOverlapBegin);
	// Mesh->OnComponentEndOverlap.AddDynamic(this, &ARPGLessonCharacter::OnOverlapEnd);
    
	// Default values for the player stats 
    MaxHealth = 100.f;
	Health = 100.f;
	MaxStamina = 350.f;
	Stamina = 350.f;
	Coins = 0;
	
	StaminaDrainRate = 100.f;
	MinSprintStamina = 50.f;

	CrouchingSpeed = 150.f;
	WalkingSpeed = 300.f;
	RunningSpeed = 600.f;
	SprintingSpeed = 1200.f;
	
	bShiftKeyDown = false;
	bIsAltPressed = false;
	bIsCtrlPressed = false;

	// Initialize Enums
	MovementStatus = EMovementStatus::EMS_Running;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}


// debug sphere, when we pickup coins , press "T" to see the location 
void ARPGLessonCharacter::ShowPickupLocation()
{
	for (FVector Location : PickupLocation)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location,25,8,FLinearColor::Green,5.f,2.f);
	}
}

void ARPGLessonCharacter::ChangeStaminaStatus(float Value)
{
	float DeltaStamina = StaminaDrainRate * Value;
	
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if(bShiftKeyDown)
		{
			if(Stamina-DeltaStamina <= MinSprintStamina) // 3
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -=DeltaStamina;
				UE_LOG(LogTemp,Warning,TEXT("ESS_NORMAL, SHIFT KEY DOWN, IF"));
			}
			else // 2 
			{
				Stamina-=DeltaStamina;
				UE_LOG(LogTemp,Warning,TEXT("ESS_NORMAL, SHIFT KEY DOWN, ELSE"));
			}
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else //shift key up
		{
			if(Stamina + DeltaStamina >= MaxStamina) // 1, 5
			{
				Stamina = MaxStamina; //we don't want to recover stamina more than the Max value is, so we increment Stamina until it reaches MaxStamina
				UE_LOG(LogTemp,Warning,TEXT("ESS_NORMAL, SHIFT KEY UP, IF"));
			}
            else // 4
            {
            	Stamina+=DeltaStamina; //refilling stamina
            	UE_LOG(LogTemp,Warning,TEXT("ESS_NORMAL, SHIFT KEY UP, ELSE"));
            }
			SetMovementStatus(EMovementStatus::EMS_Running);
		}
	break;
////////////////////////////////////////////////////////////////////////////////////////
	case EStaminaStatus::ESS_BelowMinimum:

		if(bShiftKeyDown)
		{
			if(Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0.f;
				SetMovementStatus(EMovementStatus::EMS_Running);
				UE_LOG(LogTemp,Warning,TEXT("ESS_BelowMinimum, SHIFT KEY DOWN, IF"));
			}
			else
			{
				Stamina-=DeltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
				UE_LOG(LogTemp,Warning,TEXT("ESS_BelowMinimum, SHIFT KEY DOWN, ELSE"));
			}
		}
		else // shift key up
		{
			if (Stamina+DeltaStamina>=MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina+=DeltaStamina;
				UE_LOG(LogTemp,Warning,TEXT("ESS_BelowMinimum, SHIFT KEY UP, IF"));
			}
			else
			{
				Stamina+=DeltaStamina;
				UE_LOG(LogTemp,Warning,TEXT("ESS_BelowMinimum, SHIFT KEY UP, ELSE"));
			}
			SetMovementStatus(EMovementStatus::EMS_Running);
		}
	break;
////////////////////////////////////////////////////////////////////////////////////////
case EStaminaStatus::ESS_Exhausted:
		if(bShiftKeyDown)
		{
			Stamina = 0.f;
			UE_LOG(LogTemp,Warning,TEXT("ESS_EXHAUSTED, SHIFT KEY DOWN, IF"));
		}
		else //shift key up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina+=DeltaStamina;
			UE_LOG(LogTemp,Warning,TEXT("ESS_EXHAUSTED, SHIFT KEY UP"));
		}
		SetMovementStatus(EMovementStatus::EMS_Running);
break;
////////////////////////////////////////////////////////////////////////////////////////
case EStaminaStatus::ESS_ExhaustedRecovering:
		if(Stamina+DeltaStamina>=MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			UE_LOG(LogTemp,Warning,TEXT("ESS_EXHAUSTED_RECOVERING, SHIFT KEY UP, IF"));
		}
		else
		{
			Stamina+=DeltaStamina;
			UE_LOG(LogTemp,Warning,TEXT("ESS_EXHAUSTED_RECOVERING, SHIFT KEY UP, ELSE"));
		}
		SetMovementStatus(EMovementStatus::EMS_Running);
break;

	case EStaminaStatus::ESS_MAX: break;
		default: ;
	}
}


//////////////////////////////////////////////////////////////////////////
// Input

void ARPGLessonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &ARPGLessonCharacter::AltDown);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &ARPGLessonCharacter::AltUp);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ARPGLessonCharacter::CtrlDown);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ARPGLessonCharacter::CtrlUp);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGLessonCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ARPGLessonCharacter::ShiftKeyUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGLessonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARPGLessonCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARPGLessonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARPGLessonCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARPGLessonCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARPGLessonCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ARPGLessonCharacter::OnResetVR);
}

void ARPGLessonCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("Overlap event begin!"));
}

void ARPGLessonCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Error, TEXT("Overlap event end!"));
}


void ARPGLessonCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ARPGLessonCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ChangeStaminaStatus(DeltaSeconds);
	
}



void ARPGLessonCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ARPGLessonCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ARPGLessonCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ARPGLessonCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGLessonCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGLessonCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ARPGLessonCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ARPGLessonCharacter::DecrementHealth(float HealthAmount)
{
	 if(Health>0)
	 {
	 	Health -= HealthAmount;
	 }
	 if(Health<=0)
	 {
	  	Death();
	 }
}

void ARPGLessonCharacter::IncrementHealth(float HealthAmount)
{
	if(Health>=0)
	{
		Health += HealthAmount;
	}
	
	else return;
}

void ARPGLessonCharacter::IncrementCoins(int32 CoinsAmount)
{
	Coins+=CoinsAmount;
}

void ARPGLessonCharacter::Death()
{
	UE_LOG(LogTemp,Warning,TEXT("Wasted! Your HP less than 0! "))
	//TODO: add functionality
}

void ARPGLessonCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	
	switch (MovementStatus)
	{
        //150.f
	    case EMovementStatus::EMS_Crouching:
        GetCharacterMovement()->MaxWalkSpeed = CrouchingSpeed;
	    break;
		//300.f
		case EMovementStatus::EMS_Walking:
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
		break;
        //600.f
		case EMovementStatus::EMS_Running:
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		break;
        //1200.f
		case EMovementStatus::EMS_Sprinting:
		GetCharacterMovement()->MaxWalkSpeed=SprintingSpeed;
		break;

		default:
		break;
	}
}

//TODO:implementation of crouching and walking
void ARPGLessonCharacter::AltUp()
{
	bIsAltPressed = false;
	
}

void ARPGLessonCharacter::AltDown()
{
	bIsAltPressed = true;
	
}

void ARPGLessonCharacter::CtrlUp()
{
	bIsCtrlPressed = false;
	SetMovementStatus(EMovementStatus::EMS_Running);
}

void ARPGLessonCharacter::CtrlDown()
{
	bIsCtrlPressed = true;
	SetMovementStatus(EMovementStatus::EMS_Crouching);
}

// Toggles shift variable 
void ARPGLessonCharacter::ShiftKeyDown()
{
	
	bShiftKeyDown = true;
	
	// UE_LOG(LogTemp,Warning,TEXT("Shift Key pressed down!"))
}

void ARPGLessonCharacter::ShiftKeyUp()
{
	
	bShiftKeyDown = false;
	
	// UE_LOG(LogTemp,Warning,TEXT("Shift Key pressed up!"))
}
