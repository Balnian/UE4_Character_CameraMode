// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "CR_MainCharacter.generated.h"

UCLASS()
class CHARACTER_API ACR_MainCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/**Record the world Rotation for movement */
	FRotator Direction;

   

public:
	// Sets default values for this character's properties
	ACR_MainCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/**Is in Lock Camera mode?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		bool LockCamera;

   /**Camera Zoom Rate*/
   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
      float BaseZoomRate;

   /**Max Zoom in*/
   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
      float MaxZoomIn;

   /**Max Zoom Out*/
   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
      float MaxZoomOut;

   /**Is taking Modifier*/
   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "États")
     bool IsTakingModifier;

   /**Is taking Modifier*/
   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "États")
      bool IsAttacking;

   /**Vie de base du personnage*/
   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Statistique")
     int32 BaseHealth;


   /**Damage modifier*/
   UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
      float BaseDamageModifier;



   



	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Change camera mode from lock to character direction to free cam around the character */
	void CameraMode();

	/**Update Rotation when in lockCamera mode*/
	void UpdateRotation();

   /**Adjust Camera Boom*/
   void AdjustCamBoom(float Rate);

   /**Attack Start*/
   void BeginAttack();

   /**Attack Launch*/
   void LauchAttack();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	
	
};
