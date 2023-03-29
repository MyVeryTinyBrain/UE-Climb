// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ex.h"
#include "ThirdPersonEx.h"
#include "CharacterEx.h"
#include "PlayerCharacter.generated.h"

struct MovementSlot
{
	TWeakObjectPtr<UPrimitiveComponent> hitComponent;
	FVector startFootLocation;
	FVector endLocalFootLocationOfComp;
	FQuat startRotation;
	FQuat endLocalRotationOfComp;
};

UCLASS()
class THIRDPERSON_API APlayerCharacter : public ACharacterEx
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	USpringArmComponent* springArm = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	UCameraComponent* camera = nullptr;

	FVector2D moveDirection = FVector2D::ZeroVector;

	TMap<int, MovementSlot> movementSlots;

	ECharacterState characterState = ECharacterState::Default;

	int flyModeStack = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float sphereTraceRadius = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float sphereTraceFootZOffset = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float sphereTraceFloorZOffset = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float wallCheckLength = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float stepUpCheckHeight = 270.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float stepUpCheckMaxHeight = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float stepUpForwardCheckLength = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float stepDownCheckDepth = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float stepDownMinDepth = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float stepDownForwardCheckLength = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float hangReleaseBackwardCheckLength = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float hangClimbUpForwardCheckLength = 110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float hangShimmyHeightOffset = -20.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float hangShimmySphereTraceRadius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float hangShimmyWallCheckHeight = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float hangShimmyWallCheckDepth = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float hangShimmyWallSpareDistance = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	float hangShimmyRotatePerSec = 90.0f;

	float prevRotatePerSec = 360.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	UAnimMontage* stepUpMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	UAnimMontage* stepDownMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	UAnimMontage* startGrabMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	UAnimMontage* relaseGrabMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Climb", meta = (AllowPrivateAccess = true))
	UAnimMontage* grabClimbUpMontage = nullptr;

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveVertical(float value);
	void MoveHorizontal(float value);
	void LookVertical(float value);
	void LookHorizontal(float value);
	void Interact();

	FHitResult SphereTrace(const FVector& start, const FVector& end, float radius, bool ignoreInitOverlap = true) const;
	void DebugSphereTrace(const FHitResult& hitResult, float radius, float duration = -1.0f) const;
	void DrawSphereTrace(const FVector& start, const FVector& end, float radius, const FColor& color, float duration = -1.0f) const;
	FHitResult BoxTrace(const FVector& start, const FVector& end, float halfExtent, bool ignoreInitOverlap = true) const;
	void DebugBoxTrace(const FHitResult& hitResult, float halfExtent, float duration = -1.0f) const;
	void DrawBoxTrace(const FVector& start, const FVector& end, float halfExtent, const FColor& color, float duration = -1.0f) const;
	FHitResult LineTrace(const FVector& start, const FVector& end, bool ignoreInitOverlap = true) const;
	void DebugLineTrace(const FHitResult& hitResult, float duration = -1.0f) const;

	void AddFlyStack();
	void RemoveFlyStack();

	void Climb();
	void Hang();
	void ReleaseHang();
	void SetCharacterState(ECharacterState state);

	virtual void On_ANSTranslateGauge_NofityState(const FAnimNotifyStateData& data, const FAnimNotifyStateInfo& info) override;
	virtual void On_ANSRotateGauge_NofityState(const FAnimNotifyStateData& data, const FAnimNotifyStateInfo& info) override;

private:
	UFUNCTION()
	void DefaultState_Nofity(const FAnimNotifyData& data);
};
