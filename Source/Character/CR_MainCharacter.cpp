// Fill out your copyright notice in the Description page of Project Settings.

#include "Character.h"
#include "CR_MainCharacter.h"


// Sets default values
ACR_MainCharacter::ACR_MainCharacter()
{
   // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = true;

   // Set size for collision capsule
   GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

   // set our turn rates for input
   BaseTurnRate = 45.f;
   BaseLookUpRate = 45.f;

   // Don't rotate when the controller rotates. Let that just affect the camera.
   bUseControllerRotationPitch = false;
   bUseControllerRotationYaw = false;
   bUseControllerRotationRoll = false;

   // Configure character movement
   GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
   GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
   GetCharacterMovement()->JumpZVelocity = 600.f;
   GetCharacterMovement()->AirControl = 0.2f;

   // Create a camera boom (pulls in towards the player if there is a collision)
   CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
   CameraBoom->AttachTo(RootComponent);
   CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
   CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

   // Create a follow camera
   FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
   FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
   FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

   // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
   // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)


   LockCamera = true;

   BaseZoomRate = 10.f;

   MaxZoomIn = 100.f;

   MaxZoomOut = 500.f;

   IsTakingModifier = false;

   IsAttacking = false;

   /**Combat System collision*/

   MeleeAttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeAttackBox"));
   MeleeAttackBox->RegisterComponent();
   //MeleeAttackBox->AddToRoot();
   //MeleeAttackBox->bMultiBodyOverlap = true;
   AoiAttackShere = CreateDefaultSubobject<USphereComponent>(TEXT("AoiAttackShere"));

   /**Stats*/
   BaseHealth = 100;

   BaseDamageModifier = 1.5f;

   BaseDamage = 10.f;

}

void ACR_MainCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
   // Set up gameplay key bindings
   check(InputComponent);
   InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
   InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

   InputComponent->BindAxis("MoveForward", this, &ACR_MainCharacter::MoveForward);
   InputComponent->BindAxis("MoveRight", this, &ACR_MainCharacter::MoveRight);

   // We have 2 versions of the rotation bindings to handle different kinds of devices differently
   // "turn" handles devices that provide an absolute delta, such as a mouse.
   // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
   InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
   InputComponent->BindAxis("TurnRate", this, &ACR_MainCharacter::TurnAtRate);
   InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
   InputComponent->BindAxis("LookUpRate", this, &ACR_MainCharacter::LookUpAtRate);

   // handle touch devices
   InputComponent->BindTouch(IE_Pressed, this, &ACR_MainCharacter::TouchStarted);
   InputComponent->BindTouch(IE_Released, this, &ACR_MainCharacter::TouchStopped);

   //Camera Mode
   InputComponent->BindAction("CameraMode", IE_Pressed, this, &ACR_MainCharacter::CameraMode);
   InputComponent->BindAction("CameraMode", IE_Released, this, &ACR_MainCharacter::CameraMode);

   InputComponent->BindAxis("CameraZoom", this, &ACR_MainCharacter::AdjustCamBoom);

   //Attack
   InputComponent->BindAction("Attack", IE_Pressed, this, &ACR_MainCharacter::BeginAttack);
   InputComponent->BindAction("Attack", IE_Released, this, &ACR_MainCharacter::LaunchAttack);
}


void ACR_MainCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
   // jump, but only on the first touch
   if (FingerIndex == ETouchIndex::Touch1)
   {
      Jump();
   }
}

void ACR_MainCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
   if (FingerIndex == ETouchIndex::Touch1)
   {
      StopJumping();
   }
}

void ACR_MainCharacter::TurnAtRate(float Rate)
{
   // calculate delta for this frame from the rate information
   AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACR_MainCharacter::LookUpAtRate(float Rate)
{
   // calculate delta for this frame from the rate information
   AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACR_MainCharacter::MoveForward(float Value)
{
   if ((Controller != NULL) && (Value != 0.0f))
   {
      if (LockCamera)
      {
         UpdateRotation();
      }

      // find out which way is forward
      const FRotator Rotation = Direction;//Controller->GetControlRotation();
      const FRotator YawRotation(0, Rotation.Yaw, 0);

      // get forward vector
      const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
      AddMovementInput(Direction, Value);
   }
}

void ACR_MainCharacter::MoveRight(float Value)
{
   if ((Controller != NULL) && (Value != 0.0f))
   {
      if (LockCamera)
      {
         UpdateRotation();
      }

      // find out which way is right
      const FRotator Rotation = Direction;//Controller->GetControlRotation();

      const FRotator YawRotation(0, Rotation.Yaw, 0);

      // get right vector 
      const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
      // add movement in that direction
      AddMovementInput(Direction, Value);
   }
}

void ACR_MainCharacter::UpdateRotation()
{
   if (Controller != NULL)
   {
      Direction = Controller->GetControlRotation();
      this->SetActorRotation(FRotator(0, Direction.Yaw, 0));
      //GetMesh()->SetWorldRotation(FRotator(0, Direction.Yaw + 270, 0));
   }

   /*if (GEngine)
      GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Direction.ToString());*/
}

void ACR_MainCharacter::CameraMode()
{
   LockCamera = !LockCamera;
}

void ACR_MainCharacter::AdjustCamBoom(float Rate)
{
   float future = CameraBoom->TargetArmLength + Rate*BaseZoomRate;
   if (future <= MaxZoomIn)
      future = MaxZoomIn;
   else if (future >= MaxZoomOut)
      future = MaxZoomOut;
   CameraBoom->TargetArmLength = future;

   /*if (GEngine)
      GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::SanitizeFloat(CameraBoom->TargetArmLength) );*/
}

/**Attack Start*/
void ACR_MainCharacter::BeginAttack()
{
   IsTakingModifier = true;
}

/**Attack Launch*/
void ACR_MainCharacter::LaunchAttack()
{

   IsAttacking = true;
   TArray< AActor * >  OverlappingActors;
   MeleeAttackBox->GetOverlappingActors(OverlappingActors);
   
   for (size_t i = 0; i < OverlappingActors.Num(); i++)
   {
      /*if (OverlappingActors[i]->IsA(ACR_MainCharacter::StaticClass()))
      {*/

         //ACR_MainCharacter temp = Cast(

      if (Controller != NULL)
      {
         FDamageEvent damageEvent;
         OverlappingActors[i]->TakeDamage(BaseDamage*BaseDamageModifier, damageEvent, Controller, this);
         //OverlappingActors[i]->tak

         if (GEngine)
         {
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::SanitizeFloat(BaseDamage*BaseDamageModifier));
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, OverlappingActors[i]->GetFullName());
         }
            
      }
               //this->GetAttachedActors()
               //OverlappingActors[i]->GetActorClass()
              
         
      //}

   }



}

// Called when the game starts or when spawned
void ACR_MainCharacter::BeginPlay()
{
   Super::BeginPlay();

}

// Called every frame
void ACR_MainCharacter::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);
   if (IsAttacking)
      IsAttacking = false;

}

