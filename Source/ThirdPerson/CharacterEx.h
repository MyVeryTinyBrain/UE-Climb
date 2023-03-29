// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreInclude.h"
#include "AnimInstanceEx.h"
#include "GameFramework/Character.h"
#include "CharacterEx.generated.h"

UCLASS()
class THIRDPERSON_API ACharacterEx : public ACharacter
{
	GENERATED_BODY()

protected:
	FRotator prevRotation = FRotator::ZeroRotator;
	FRotator targetRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Rotate, meta = (AllowPrivateAccess = true))
	float rotatePerSec = 360.0f;

public:
	ACharacterEx();

	void SetRotateSpeed(float RotatePerSec);
	void SetRotationImmediate(const FRotator& rotator);
	void SetRotationImmediate(const FQuat& rotation);
	void SetFootLocation(const FVector& location, bool sweep = false);
	void SetTopLocation(const FVector& location, bool sweep = false);
	FVector GetFootLocation() const;
	FVector GetTopLocation() const;

	UAnimInstanceEx* GetAnimInstanceEx() const { return Cast<UAnimInstanceEx>(GetMesh()->GetAnimInstance()); }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void ANMovementMode_Nofity(const FAnimNotifyData& data);

	UFUNCTION()
	void ANSTranslateGauge_NofityState(const FAnimNotifyStateData& data);

	virtual void On_ANSTranslateGauge_NofityState(const FAnimNotifyStateData& data, const FAnimNotifyStateInfo& info) {}

	UFUNCTION()
	void ANSRotateGauge_NofityState(const FAnimNotifyStateData& data);

	virtual void On_ANSRotateGauge_NofityState(const FAnimNotifyStateData& data, const FAnimNotifyStateInfo& info) {}
};
