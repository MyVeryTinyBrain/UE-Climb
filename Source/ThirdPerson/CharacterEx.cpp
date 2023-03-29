// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEx.h"
#include "ANSTranslateGauge.h"
#include "Ex.h"

ACharacterEx::ACharacterEx()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -100), FRotator(0, -90, 0));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

void ACharacterEx::SetRotateSpeed(float RotatePerSec)
{
	this->rotatePerSec = RotatePerSec;
}

void ACharacterEx::SetRotationImmediate(const FRotator& rotator)
{
	prevRotation = rotator;
	targetRotation = rotator;
	SetActorRotation(rotator);
}

void ACharacterEx::SetRotationImmediate(const FQuat& rotation)
{
	SetRotationImmediate(rotation.Rotator());
}

void ACharacterEx::SetFootLocation(const FVector& location, bool sweep)
{
	FVector center = location + FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetActorLocation(center, sweep);
}

void ACharacterEx::SetTopLocation(const FVector& location, bool sweep)
{
	FVector center = location + FVector::DownVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetActorLocation(center, sweep);
}

FVector ACharacterEx::GetFootLocation() const
{
	return GetActorLocation() + FVector::DownVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

FVector ACharacterEx::GetTopLocation() const
{
	return GetActorLocation() + FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void ACharacterEx::BeginPlay()
{
	Super::BeginPlay();
	
	prevRotation = GetActorRotation();
	targetRotation = prevRotation;
}

void ACharacterEx::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if rotation was changed?
	if (prevRotation != GetActorRotation())
	{
		targetRotation = GetActorRotation();
		SetActorRotation(prevRotation);
	}

	// Rotate to target
	FVector from = GetActorForwardVector();
	FVector to = targetRotation.RotateVector(FVector::ForwardVector);
	float betweenAngle = FMathEx::BetweenAngle(from, to);
	if (abs(betweenAngle) > 0.1f)
	{
		float maxDeltaAngle = rotatePerSec * DeltaTime;
		FQuat newRotation = FMathEx::RotateTowards(GetActorRotation().Quaternion(), targetRotation.Quaternion(), maxDeltaAngle);
		SetActorRotation(newRotation);
	}

	// Cache current rotation
	prevRotation = GetActorRotation();
}

void ACharacterEx::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacterEx::ANMovementMode_Nofity(const FAnimNotifyData& data)
{
}

void ACharacterEx::ANSTranslateGauge_NofityState(const FAnimNotifyStateData& data)
{
	FAnimNotifyStateInfo info = data.AnimNotifyState->GetAnimNotifyStateInfo(data);
	if (!info.isValid || !info.isInMontage)
		return;

	On_ANSTranslateGauge_NofityState(data, info);
}

void ACharacterEx::ANSRotateGauge_NofityState(const FAnimNotifyStateData& data)
{
	FAnimNotifyStateInfo info = data.AnimNotifyState->GetAnimNotifyStateInfo(data);
	if (!info.isValid || !info.isInMontage)
		return;

	On_ANSRotateGauge_NofityState(data, info);
}
