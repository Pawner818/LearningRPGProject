// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGLessonCharacter.h"
#include "Math/UnrealMathUtility.h"
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
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h" 



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

	// Default values for the player stats 
    MaxHealth = 100.f;
	Health = 100.f;
	
	Coins = 0;

	MaxStamina = 350.f;
	MinStamina = 0.f;
	CurrentStamina = 350.f;
	StaminaDrainRate = 100.f;
	StaminaExhausted = 50.f;
	bCouldWeDrainStamina = false;

	CrouchingSpeed = 150.f;
	WalkingSpeed = 300.f;
	RunningSpeed = 600.f;
	SprintingSpeed = 1200.f;
	
	bShiftKeyDown = false;
	bIsAltPressed = false;
	bIsCtrlPressed = false;

	// Initialize ENUMS
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

void ARPGLessonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Keyboard input
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARPGLessonCharacter::SpaceKeyDown);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARPGLessonCharacter::SpaceKeyUp);

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

	// Mouse input
	PlayerInputComponent->BindAction("LMB", IE_Pressed,this,&ARPGLessonCharacter::LMBPressed);
	PlayerInputComponent->BindAction("LMB", IE_Released,this,&ARPGLessonCharacter::LMBReleased);

	PlayerInputComponent->BindAction("RMB", IE_Pressed,this,&ARPGLessonCharacter::RMBPressed);
	PlayerInputComponent->BindAction("RMB", IE_Released,this,&ARPGLessonCharacter::RMBReleased);
}

void ARPGLessonCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ARPGLessonCharacter::Attack()
{
	if(!bAttacking)
	{
		bAttacking = true;
		UAnimInstance*AnimInstance=GetMesh()->GetAnimInstance();
		if(AnimInstance && CombatMontage)
		{
			int32 MontageSection = FMath::RandRange(0,3);
			switch (MontageSection)
			{
				case 0:
				AnimInstance->Montage_Play(CombatMontage, 1.0f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;
				
				case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.0f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;
				
				case 2:
				AnimInstance->Montage_Play(CombatMontage, 1.0f);
				AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);
				break;
				
				case 3:
				AnimInstance->Montage_Play(CombatMontage, 1.0f);
				AnimInstance->Montage_JumpToSection(FName("Attack_Range"), CombatMontage);
				break;
				
				default:
				break;
			}
			
		}
	}
	
}



void ARPGLessonCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MovementStatusUpdating(DeltaSeconds);

	StaminaStatusUpdating(DeltaSeconds);
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
	if ((Controller != NULL) && (Value != 0.0f) )
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
	if ( (Controller != NULL) && (Value != 0.0f)  )
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
        // speed is 150.f
	    case EMovementStatus::EMS_Crouching:
        GetCharacterMovement()->MaxWalkSpeed = CrouchingSpeed;
	    break;
		// speed is 300.f
		case EMovementStatus::EMS_Walking:
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
		break;
        // speed is 600.f
		case EMovementStatus::EMS_Running:
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		break;
        // speed is 1200.f
		case EMovementStatus::EMS_Sprinting:
		GetCharacterMovement()->MaxWalkSpeed=SprintingSpeed;
		break;

		default:
		break;
	}
}

// Updating movement status updated every frame
void ARPGLessonCharacter::MovementStatusUpdating(float DeltaValue)
{
	// Switching moving status according to the currently clicked buttons - Shift-Sprint, Ctrl-Walk, Alt-Crouch
	switch(MovementStatus)
	{
		/* 
		* Movement Status : Running (Default)
		*/
	
		// By default we are in this status. When we press WASD, we are able to run. Also, this is a toggle to
		// other movement statuses.
		
		case EMovementStatus::EMS_Running: 
			{
				// UE_LOG(LogTemp, Warning, TEXT("EMS RUNNING CASE"));
				
					if(bShiftKeyDown && StaminaStatus!=EStaminaStatus::ESS_Recovering)
					{
						// UE_LOG(LogTemp, Warning, TEXT("EMS RUNNING CASE SHIFT KEY DOWN"));
						SetMovementStatus(EMovementStatus::EMS_Sprinting);
					}
				    
					if(bIsCtrlPressed)
					{
						// UE_LOG(LogTemp, Warning, TEXT("EMS RUNNING CASE CTRL KEY DOWN"));
						SetMovementStatus(EMovementStatus::EMS_Crouching);
					}
					
					if (bIsAltPressed)
					{
						// UE_LOG(LogTemp, Warning, TEXT("EMS RUNNING CASE ALT KEY DOWN"));
						SetMovementStatus(EMovementStatus::EMS_Walking);
					}	
			}
			
		break;

		/* 
		* Movement Status : Sprinting
		*/

		case EMovementStatus::EMS_Sprinting:
			{
				// UE_LOG(LogTemp, Warning, TEXT("EMS SPRINTING CASE"));
				bCouldWeDrainStamina=true;
			}
		break;

		/* 
		* Movement Status : Crouching
		*/
		
		case EMovementStatus::EMS_Crouching:
			{
				// UE_LOG(LogTemp, Warning, TEXT("EMS CROUCHING CASE"));
				if(bIsCtrlPressed)
				{
					SetMovementStatus(EMovementStatus::EMS_Crouching);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Running);
				}
				
			}
		break;
		
		/* 
		* Movement Status : Walking
		*/
		
		case EMovementStatus::EMS_Walking:
			{
				// UE_LOG(LogTemp, Warning, TEXT("EMS WALKING CASE"));
				if(bIsAltPressed)
				{
					SetMovementStatus(EMovementStatus::EMS_Walking);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Running);
				}
			}
		break;

		default:
		break;
	}
}

void ARPGLessonCharacter::SetStaminaStatus(EStaminaStatus Status)
{
	StaminaStatus = Status;
	switch (StaminaStatus)
	{
		case EStaminaStatus::ESS_Normal:
		break;
		case EStaminaStatus::ESS_BelowMinimum:
		break;
		case EStaminaStatus::ESS_Recovering:
		break;

		default:
		break;
	}
}

void ARPGLessonCharacter::StaminaStatusUpdating(float DeltaValue)
{
	float DeltaStamina = StaminaDrainRate * DeltaValue;

	/*
	 * When we are in the normal state (not moving), we have two options.
	 * The first allows you to switch to a different movement status (sprint) and start using mana when we press the shift key.
	 * If we do not press the key, we go into Recovery mode and wait until the stamina indicator is complete.
	 * 
	 * There is also a status when the stamina indicator changes its color to green - BelowMin status.
	 * It occurs when the stamina indicator drops less the StaminaExhausted value (by def. it's 50.f)	
	 */
	
	switch (StaminaStatus)
	{

	/* 
	 * Stamina Status : NORMAL
	*/
		
	case EStaminaStatus::ESS_Normal:
		
		if(bShiftKeyDown)
		
		{
			if(CurrentStamina>=StaminaExhausted)
			{
				// UE_LOG(LogTemp,Warning,TEXT("ESS_NORMAL, SKD IF"));  
				CurrentStamina-=DeltaStamina;
			}
			else
			{
				// UE_LOG(LogTemp,Warning,TEXT("ESS_NORMAL, SKD ELSE")); 
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
			}
		}
		else //SKUP
		{
			if(CurrentStamina<MaxStamina)
			{
				// UE_LOG(LogTemp,Warning,TEXT("ESS_NORMAL, SKUP IF"));
				if(CurrentStamina<MaxStamina && CurrentStamina>=StaminaExhausted)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Recovering);
				}
				
			}
			else
			{
				// UE_LOG(LogTemp,Warning,TEXT("ESS_NORMAL, SKUP ELSE"));
				CurrentStamina=MaxStamina;                 
			}
		}
		
	break;
		
	/* 
	* Stamina Status : BelowMinimum
	*/
		
	case EStaminaStatus::ESS_BelowMinimum:
		
		if(bShiftKeyDown)
		{
			if(CurrentStamina > MinStamina && CurrentStamina <= StaminaExhausted) 
			{
				// UE_LOG(LogTemp,Warning,TEXT("ESS_BELOWMINIMUM, SKD IF"));
				CurrentStamina-=DeltaStamina;
			}
			else 
			{
			
				// UE_LOG(LogTemp,Warning,TEXT("ESS_BELOWMINIMUM, SKD CURRENTSTAMINA == MINSTAMINA")); 
				SetStaminaStatus(EStaminaStatus::ESS_Recovering);
			}
		}
		else // SKUP
		{
			
			if(CurrentStamina>MinStamina && CurrentStamina<=StaminaExhausted)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Recovering);
			}
		}
	break;

	/* 
	* Stamina Status : Recovering
	*/
		
    case EStaminaStatus::ESS_Recovering:
		// UE_LOG(LogTemp,Warning,TEXT("RECOVERING ENTRY"));
		if(bShiftKeyDown)
		{
			if(CurrentStamina<MaxStamina)
			{
				// UE_LOG(LogTemp,Warning,TEXT("RECOVERING IF IF"));
				CurrentStamina+=DeltaStamina;
			}
			else
			{
				// UE_LOG(LogTemp,Warning,TEXT("RECOVERING IF ELSE"));
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
		}
		
		else //SKUP
		{
			CurrentStamina+=DeltaStamina;
			if(CurrentStamina>=MaxStamina)
			{
				CurrentStamina=MaxStamina;
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}
		}
		SetMovementStatus(EMovementStatus::EMS_Running);
		
break;

	case EStaminaStatus::ESS_MAX: break;
		default: break;
	}
}

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
	
}

void ARPGLessonCharacter::CtrlDown()
{
	bIsCtrlPressed = true;
	
}

void ARPGLessonCharacter::ShiftKeyDown()
{
	bShiftKeyDown = true;
	
}

void ARPGLessonCharacter::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void ARPGLessonCharacter::SpaceKeyDown()
{
	
	Jump();
}

void ARPGLessonCharacter::SpaceKeyUp()
{
	StopJumping();
}

void ARPGLessonCharacter::LMBPressed()
{
	bIsLMBPressed = true;
	Attack();
}

void ARPGLessonCharacter::LMBReleased()
{
	bIsLMBPressed = false;
}

void ARPGLessonCharacter::RMBPressed()
{
	bIsRMBPressed = true;
}

void ARPGLessonCharacter::RMBReleased()
{
	bIsRMBPressed = false;
}

void ARPGLessonCharacter::AttackEnd()
{
	bAttacking = false;
}