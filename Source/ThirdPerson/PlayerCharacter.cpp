// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "ThirdPersonAnim.h"
#include "ANSTranslateGauge.h"
#include "ANSRotateGauge.h"
#include <Runtime/Engine/Private/KismetTraceUtils.h>
#include <Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>

APlayerCharacter::APlayerCharacter() : ACharacterEx()
{
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> skm(TEXT("/Game/Characters/YBot/SKM_YBot.SKM_YBot"));
	GetMesh()->SetSkeletalMesh(skm.Object);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -87), FRotator(0, -90, 0));

	ConstructorHelpers::FClassFinder<UAnimInstance> ai(TEXT("/Game/Characters/YBot/AB_YBot.AB_YBot_C"));
	GetMesh()->SetAnimInstanceClass(ai.Class);
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 85.0f);
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	springArm->SetupAttachment(RootComponent);
	springArm->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	springArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(springArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MaxFlySpeed = 70.0f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	switch (characterState)
	{
		case ECharacterState::Default:
		{
			// 캐릭터를 카메라 방향으로 회전
			if (moveDirection.Length() > 0.0f)
			{
				float yaw = camera->GetComponentRotation().GetComponentForAxis(EAxis::Z);
				// Round float
				yaw = float(int(yaw * 10.0f)) / 10.0f;
				targetRotation.Yaw = yaw;
			}
		}
		break;
	}

	Super::Tick(DeltaTime);

	// Normalized movement input direction
	moveDirection.Normalize();

	// Set Anim instnace properties
	UThirdPersonAnim* anim = Cast<UThirdPersonAnim>(GetMesh()->GetAnimInstance());
	float normalizedSpeed = FVector::VectorPlaneProject(GetCharacterMovement()->Velocity, FVector::UpVector).Length() / GetCharacterMovement()->GetMaxSpeed();
	anim->SetNormalizedSpeed(normalizedSpeed);
	// Apply move direction into anim instance
	anim->SetMoveDirection(moveDirection);

	switch (characterState)
	{
		case ECharacterState::Default:
		{
			// Add Movement input
			AddMovementInput(GetActorForwardVector(), moveDirection.Y);
			AddMovementInput(GetActorRightVector(), moveDirection.X);

			Climb();
		}
		break;
		case ECharacterState::Hang:
		{
			Hang();
		}
		break;
	}

	// Reset move direction
	moveDirection = FVector2D::ZeroVector;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveVertical"), this, &APlayerCharacter::MoveVertical);
	PlayerInputComponent->BindAxis(TEXT("MoveHorizontal"), this, &APlayerCharacter::MoveHorizontal);
	PlayerInputComponent->BindAxis(TEXT("LookVertical"), this, &APlayerCharacter::LookVertical);
	PlayerInputComponent->BindAxis(TEXT("LookHorizontal"), this, &APlayerCharacter::LookHorizontal);
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Interact);
}

void APlayerCharacter::MoveVertical(float value)
{
	if (GetAnimInstanceEx()->Montage_IsActive(nullptr))
		return;

	moveDirection.Y = value;
}

void APlayerCharacter::MoveHorizontal(float value)
{
	if (GetAnimInstanceEx()->Montage_IsActive(nullptr))
		return;

	moveDirection.X = value;
}

void APlayerCharacter::LookVertical(float value)
{
	AddControllerPitchInput(-value);
}

void APlayerCharacter::LookHorizontal(float value)
{
	AddControllerYawInput(value);
}

void APlayerCharacter::Interact()
{
	if (GetAnimInstanceEx()->Montage_IsActive(nullptr))
		return;

	switch (characterState)
	{
		case ECharacterState::Hang:
			ReleaseHang();
			break;
	}
}

FHitResult APlayerCharacter::SphereTrace(const FVector& start, const FVector& end, float radius, bool ignoreInitOverlap) const
{
	FHitResult hitResult;
	FCollisionObjectQueryParams params1(ECC_TO_BITFIELD(ECollisionChannel::ECC_WorldStatic) | ECC_TO_BITFIELD(ECollisionChannel::ECC_WorldDynamic));
	FCollisionShape shape;
	shape.SetSphere(radius);
	FCollisionQueryParams params2(NAME_None, true, this);
	params2.bIgnoreTouches = true;
	params2.AddIgnoredActor(this);
	GetWorld()->SweepSingleByObjectType(hitResult, start, end, FQuat::Identity, params1, shape, params2);
	if (!hitResult.bBlockingHit)
	{
		hitResult.Distance = FVector::Dist(start, end);
	}
	if (ignoreInitOverlap)
	{
		hitResult.bBlockingHit = hitResult.Distance == 0 ? false : hitResult.bBlockingHit;
	}
	return hitResult;
}

void APlayerCharacter::DebugSphereTrace(const FHitResult& hitResult, float radius, float duration) const
{
	if (hitResult.bBlockingHit)
	{
		DrawSphereTrace(hitResult.TraceStart, hitResult.TraceEnd, radius, FColor::Red, duration);
		DrawDebugPoint(GetWorld(), hitResult.ImpactPoint, 10, FColor::Red, false, duration);
	}
	else
	{
		DrawSphereTrace(hitResult.TraceStart, hitResult.TraceEnd, radius, FColor::Green, duration);
	}
}

void APlayerCharacter::DrawSphereTrace(const FVector& start, const FVector& end, float radius, const FColor& color, float duration) const
{
	FVector TraceVec = end - start;
	float Dist = TraceVec.Size();

	FVector Center = start + TraceVec * 0.5f;
	float HalfHeight = (Dist * 0.5f) + radius;

	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	DrawDebugCapsule(GetWorld(), Center, HalfHeight, radius, CapsuleRot, color, false, duration);
}

FHitResult APlayerCharacter::BoxTrace(const FVector& start, const FVector& end, float halfExtent, bool ignoreInitOverlap) const
{
	FHitResult hitResult;
	FCollisionObjectQueryParams params1(ECC_TO_BITFIELD(ECollisionChannel::ECC_WorldStatic) | ECC_TO_BITFIELD(ECollisionChannel::ECC_WorldDynamic));
	FCollisionShape shape;
	shape.SetBox(FVector3f::OneVector * halfExtent);
	FCollisionQueryParams params2(NAME_None, true, this);
	params2.bIgnoreTouches = true;
	params2.AddIgnoredActor(this);
	FQuat rotation = FRotationMatrix::MakeFromX(end - start).ToQuat();
	GetWorld()->SweepSingleByObjectType(hitResult, start, end, rotation, params1, shape, params2);
	if (!hitResult.bBlockingHit)
	{
		hitResult.Distance = FVector::Dist(start, end);
	}
	if (ignoreInitOverlap)
	{
		hitResult.bBlockingHit = hitResult.Distance == 0 ? false : hitResult.bBlockingHit;
	}
	return hitResult;
}

void APlayerCharacter::DebugBoxTrace(const FHitResult& hitResult, float halfExtent, float duration) const
{
	if (hitResult.bBlockingHit)
	{
		DrawBoxTrace(hitResult.TraceStart, hitResult.TraceEnd, halfExtent, FColor::Red, duration);
		DrawDebugPoint(GetWorld(), hitResult.ImpactPoint, 10, FColor::Red, false, duration);
		DrawDebugLine(GetWorld(), hitResult.ImpactPoint, hitResult.ImpactPoint + hitResult.ImpactNormal * 100.0f, FColor::Blue, false, duration);
	}
	else
	{
		DrawBoxTrace(hitResult.TraceStart, hitResult.TraceEnd, halfExtent, FColor::Green, duration);
	}
}

void APlayerCharacter::DrawBoxTrace(const FVector& start, const FVector& end, float halfExtent, const FColor& color, float duration) const
{
	float len = (end - start).Length() + halfExtent * 2.0f;
	FVector Center = (start + end) * 0.5f;
	DrawDebugBox(GetWorld(), Center, FVector(len * 0.5f, halfExtent, halfExtent), color, false, duration);
}

FHitResult APlayerCharacter::LineTrace(const FVector& start, const FVector& end, bool ignoreInitOverlap) const
{
	FHitResult hitResult;
	FCollisionObjectQueryParams params1(ECC_TO_BITFIELD(ECollisionChannel::ECC_WorldStatic) | ECC_TO_BITFIELD(ECollisionChannel::ECC_WorldDynamic));
	FCollisionQueryParams params2(NAME_None, true, this);
	params2.bIgnoreTouches = true;
	params2.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, params1, params2);
	if (!hitResult.bBlockingHit)
	{
		hitResult.Distance = FVector::Dist(start, end);
	}
	if (ignoreInitOverlap)
	{
		hitResult.bBlockingHit = hitResult.Distance == 0 ? false : hitResult.bBlockingHit;
	}
	return hitResult;
}

void APlayerCharacter::DebugLineTrace(const FHitResult& hitResult, float duration) const
{
	if (hitResult.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), hitResult.TraceStart, hitResult.TraceEnd, FColor::Red, false, duration);
		DrawDebugPoint(GetWorld(), hitResult.ImpactPoint, 10, FColor::Red, false, duration);
	}
	else
	{
		DrawDebugLine(GetWorld(), hitResult.TraceStart, hitResult.TraceEnd, FColor::Green, false, duration);
	}
}

void APlayerCharacter::AddFlyStack()
{
	flyModeStack++;
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying)
		return;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void APlayerCharacter::RemoveFlyStack()
{
	int prev = flyModeStack;
	flyModeStack = FMath::Clamp(flyModeStack - 1, 0, INT_MAX);
	if (prev == flyModeStack)
		return;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void APlayerCharacter::Climb()
{
	if (GetAnimInstanceEx()->Montage_IsActive(nullptr))
		return;

	// 이동 입력이 있을 때만 등반이 가능
	if (moveDirection.SquaredLength() < 0.01f)
		return;

	FVector sweepDirection2D = GetTransform().TransformVector(FVector(moveDirection.Y, moveDirection.X, 0));

	// Sweep to moving direction for wall check
	FVector wallCheckStart = GetFootLocation();
	wallCheckStart.Z = wallCheckStart.Z + GetCapsuleComponent()->GetScaledCapsuleRadius() + sphereTraceFootZOffset;
	FVector wallCheckEnd = wallCheckStart + sweepDirection2D * wallCheckLength;
	FHitResult wallCheckHit = SphereTrace(wallCheckStart, wallCheckEnd, sphereTraceRadius, false);
	DebugSphereTrace(wallCheckHit, sphereTraceRadius);

	// Sweep to moving direction for lower floor check
	float lowerFloorCheckStartHeight = GetCapsuleComponent()->GetScaledCapsuleRadius() + sphereTraceFootZOffset;
	FVector lowerFloorCheckStart = GetFootLocation();
	lowerFloorCheckStart += sweepDirection2D * (GetCapsuleComponent()->GetScaledCapsuleRadius() + sphereTraceRadius + 1.0f/*adjust*/);
	lowerFloorCheckStart.Z += lowerFloorCheckStartHeight;
	FVector lowerFloorCheckEnd = lowerFloorCheckStart + FVector::DownVector * stepDownCheckDepth;
	FHitResult lowerFloorCheckHit = SphereTrace(lowerFloorCheckStart, lowerFloorCheckEnd, sphereTraceRadius);
	float lowerFloorAngle = FMathEx::BetweenAngle(lowerFloorCheckHit.ImpactNormal, FVector::UpVector);
	DebugSphereTrace(lowerFloorCheckHit, sphereTraceRadius);

	// Can climb if forward or strafe input press
	if (FVector2D::DotProduct(moveDirection, FVector2D(0, 1)) >= 0 && wallCheckHit.bBlockingHit)
	{
		// Sweep to down for floor check
		FVector floorCheckStart = wallCheckHit.ImpactPoint + FVector::UpVector * stepUpCheckHeight;
		FVector floorCheckEnd = wallCheckHit.ImpactPoint;
		FHitResult floorCheckHit = SphereTrace(floorCheckStart, floorCheckEnd, sphereTraceRadius);
		DebugSphereTrace(floorCheckHit, sphereTraceRadius, 3);
		float floorAngle = FMathEx::BetweenAngle(floorCheckHit.ImpactNormal, FVector::UpVector);

		if (floorCheckHit.bBlockingHit && floorAngle <= GetCharacterMovement()->GetWalkableFloorAngle())
		{
			float floorToUpperGround = stepUpCheckHeight - floorCheckHit.Distance;
			if (floorToUpperGround < stepUpCheckMaxHeight)
			{
				// Step up

				MovementSlot& slot0 = movementSlots.FindOrAdd(0);
				// Set hit component
				slot0.hitComponent = floorCheckHit.Component;
				// Set world variables
				slot0.endLocalFootLocationOfComp = floorCheckHit.ImpactPoint;
				slot0.endLocalRotationOfComp = FRotationMatrix::MakeFromX(-FVector::VectorPlaneProject(wallCheckHit.ImpactNormal, FVector::UpVector).GetSafeNormal()).ToQuat();

				// Sweep to inversed impacted normal for move to forward on cliff
				FVector floorForwardCheckStart = floorCheckHit.ImpactPoint + FVector::UpVector * (sphereTraceRadius + sphereTraceFloorZOffset);
				FVector floorForwardCheckEnd = floorForwardCheckStart - wallCheckHit.ImpactNormal * stepUpForwardCheckLength;
				FHitResult floorForwardCheckHit = SphereTrace(floorForwardCheckStart, floorForwardCheckEnd, sphereTraceRadius);
				DebugSphereTrace(floorForwardCheckHit, sphereTraceRadius, 3);
				float forwardDistance = FMath::Clamp(floorForwardCheckHit.Distance - GetCapsuleComponent()->GetScaledCapsuleRadius(), 0, MAX_FLT);
				slot0.endLocalFootLocationOfComp -= wallCheckHit.ImpactNormal * forwardDistance;

				// Apply end transform variables
				slot0.endLocalFootLocationOfComp = slot0.hitComponent->GetComponentTransform().InverseTransformPosition(slot0.endLocalFootLocationOfComp);
				slot0.endLocalRotationOfComp = slot0.hitComponent->GetComponentTransform().InverseTransformRotation(slot0.endLocalRotationOfComp);

				// Play step up montage
				GetAnimInstanceEx()->Montage_Play(stepUpMontage);
			}
			else
			{
				// A vector to move backwards by the radius of the capsule
				FVector pullVector = wallCheckHit.ImpactNormal * (GetCapsuleComponent()->GetScaledCapsuleRadius());
				FVector footEnd = floorCheckHit.ImpactPoint + pullVector;

				// Check hang path is not blocked
				FVector validHangCheckStart = GetActorLocation();
				FVector validHangCheckEnd = footEnd;
				FHitResult validHangeCheckHit = LineTrace(validHangCheckStart, validHangCheckEnd);
				DebugLineTrace(validHangeCheckHit, 3);
				if (!validHangeCheckHit.bBlockingHit)
				{
					// Hang

					// Set Character state to hang
					// It's change capsule radius to better to hang
					// And disable collision
					SetCharacterState(ECharacterState::Hang);

					MovementSlot& slot0 = movementSlots.FindOrAdd(0);

					// Set hit component
					slot0.hitComponent = floorCheckHit.Component;
					// Set world variables
					slot0.endLocalFootLocationOfComp = footEnd;
					slot0.endLocalRotationOfComp = FRotationMatrix::MakeFromX(-FVector::VectorPlaneProject(wallCheckHit.ImpactNormal, FVector::UpVector).GetSafeNormal()).ToQuat();

					// Apply end transform variables
					slot0.endLocalFootLocationOfComp = slot0.hitComponent->GetComponentTransform().InverseTransformPosition(slot0.endLocalFootLocationOfComp);
					slot0.endLocalRotationOfComp = slot0.hitComponent->GetComponentTransform().InverseTransformRotation(slot0.endLocalRotationOfComp);

					// Play step up montage
					GetAnimInstanceEx()->Montage_Play(startGrabMontage);

					// Attach to component
					FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
					AttachToComponent(slot0.hitComponent.Get(), rules);
				}
			}
		}
	}
	else if (lowerFloorCheckHit.bBlockingHit && lowerFloorAngle <= GetCharacterMovement()->GetWalkableFloorAngle() && lowerFloorCheckHit.Distance > stepDownMinDepth + lowerFloorCheckStartHeight)
	{
		MovementSlot& slot0 = movementSlots.FindOrAdd(0);

		// Set hit component
		slot0.hitComponent = lowerFloorCheckHit.Component;
		// Set world variables
		slot0.endLocalFootLocationOfComp = lowerFloorCheckHit.ImpactPoint;
		slot0.endLocalRotationOfComp = FRotationMatrix::MakeFromX(sweepDirection2D).ToQuat();

		// Sweep to moving direction for move to forward on floor
		FVector floorForwardCheckStart = lowerFloorCheckHit.ImpactPoint + FVector::UpVector * (sphereTraceRadius + sphereTraceFloorZOffset);
		FVector floorForwardCheckEnd = floorForwardCheckStart + sweepDirection2D * stepDownForwardCheckLength;
		FHitResult floorForwardCheckHit = SphereTrace(floorForwardCheckStart, floorForwardCheckEnd, sphereTraceRadius);
		DebugSphereTrace(floorForwardCheckHit, sphereTraceRadius, 3);
		float forwardDistance = FMath::Clamp(floorForwardCheckHit.Distance - GetCapsuleComponent()->GetScaledCapsuleRadius(), 0, MAX_FLT);
		slot0.endLocalFootLocationOfComp += sweepDirection2D * forwardDistance;

		// Apply end transform variables
		slot0.endLocalFootLocationOfComp = slot0.hitComponent->GetComponentTransform().InverseTransformPosition(slot0.endLocalFootLocationOfComp);
		slot0.endLocalRotationOfComp = slot0.hitComponent->GetComponentTransform().InverseTransformRotation(slot0.endLocalRotationOfComp);

		// Play step down montage
		GetAnimInstanceEx()->Montage_Play(stepDownMontage);
	}
}

void APlayerCharacter::Hang()
{
	if (GetAnimInstanceEx()->IsAnyMontagePlaying())
		return;

	// Can climb during input
	if (moveDirection.SquaredLength() < 0.01f)
		return;

	// Can climb if forward input press
	if (FVector2D::DotProduct(moveDirection, FVector2D(0, 1)) > 0)
	{
		// Sweep from up to upper ground for climb up
		float capsuleHeightCheck = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + GetCapsuleComponent()->GetScaledCapsuleRadius() * 2;
		FVector upperFloorCheckXY = GetTopLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
		FVector upperFloorCheckStart = upperFloorCheckXY + FVector::UpVector * capsuleHeightCheck;
		FVector upperFloorCheckEnd = upperFloorCheckXY + FVector::DownVector * 10.0f/*for adjust*/;
		FHitResult upperFloorCheckHit = SphereTrace(upperFloorCheckStart, upperFloorCheckEnd, sphereTraceRadius, false);
		DebugSphereTrace(upperFloorCheckHit, sphereTraceRadius);
		bool canClimbUp = true;
		float floorAngle = FMathEx::BetweenAngle(upperFloorCheckHit.ImpactNormal, FVector::UpVector);

		if (!upperFloorCheckHit.bBlockingHit)
			canClimbUp = false;
		// Hasn't space for put character's capsule
		else if (upperFloorCheckHit.Distance < GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
			canClimbUp = false;
		else if (floorAngle > GetCharacterMovement()->GetWalkableFloorAngle())
			canClimbUp = false;

		if (canClimbUp)
		{
			MovementSlot* slot0 = &movementSlots.FindOrAdd(0);
			MovementSlot* slot1 = &movementSlots.FindOrAdd(1);

			// Set hit component
			slot0->hitComponent = slot1->hitComponent = upperFloorCheckHit.Component;
			// Set world variables
			slot0->endLocalFootLocationOfComp.X = GetFootLocation().X;
			slot0->endLocalFootLocationOfComp.Y = GetFootLocation().Y;
			slot0->endLocalFootLocationOfComp.Z = upperFloorCheckHit.ImpactPoint.Z;
			slot0->endLocalRotationOfComp = GetActorRotation().Quaternion();
			*slot1 = *slot0;

			// Sweep to forward for move to forward on floor
			FVector floorForwardCheckStart = upperFloorCheckHit.ImpactPoint + FVector::UpVector * (sphereTraceRadius + sphereTraceFloorZOffset);
			FVector floorForwardCheckEnd = floorForwardCheckStart + GetActorForwardVector() * hangClimbUpForwardCheckLength;
			FHitResult floorForwardCheckHit = SphereTrace(floorForwardCheckStart, floorForwardCheckEnd, sphereTraceRadius);
			DebugSphereTrace(floorForwardCheckHit, sphereTraceRadius, 3);
			float forwardDistance = FMath::Clamp(floorForwardCheckHit.Distance - GetCapsuleComponent()->GetScaledCapsuleRadius(), 0, MAX_FLT);
			slot1->endLocalFootLocationOfComp += GetActorForwardVector() * forwardDistance;

			// Apply end transform variables
			slot0->endLocalFootLocationOfComp = slot0->hitComponent->GetComponentTransform().InverseTransformPosition(slot0->endLocalFootLocationOfComp);
			slot0->endLocalRotationOfComp = slot0->hitComponent->GetComponentTransform().InverseTransformRotation(slot0->endLocalRotationOfComp);
			slot1->endLocalFootLocationOfComp = slot1->hitComponent->GetComponentTransform().InverseTransformPosition(slot1->endLocalFootLocationOfComp);
			slot1->endLocalRotationOfComp = slot1->hitComponent->GetComponentTransform().InverseTransformRotation(slot1->endLocalRotationOfComp);

			// Play climb up montage
			GetAnimInstanceEx()->Montage_Play(grabClimbUpMontage);
		}
	}
	// Can shimmy if horizontal input press
	else if (FVector2D::DotProduct(moveDirection, FVector2D(1, 0)) > 0 || FVector2D::DotProduct(moveDirection, FVector2D(-1, 0)) > 0)
	{
		FVector sideSweepDirection2D = GetTransform().TransformVector(FVector(moveDirection.Y, moveDirection.X, 0));

		// Sweep to front wall for strafe
		float addLengthToSideMove = GetCapsuleComponent()->GetScaledCapsuleRadius() + hangShimmySphereTraceRadius;
		FVector sideWallCheckStartSide = sideSweepDirection2D * addLengthToSideMove;
		FVector siwWallCheckStart = GetActorLocation() + FVector::UpVector * hangShimmyWallCheckHeight + sideWallCheckStartSide;
		FVector sideWallCheckEnd = siwWallCheckStart + GetActorForwardVector() * hangShimmyWallCheckDepth;
		FHitResult sideWallCheckHit = SphereTrace(siwWallCheckStart, sideWallCheckEnd, hangShimmySphereTraceRadius, false);
		DebugSphereTrace(sideWallCheckHit, hangShimmySphereTraceRadius);

		if (sideWallCheckHit.bBlockingHit)
		{
			// Sweep from up to upper ground for ground check
			float capsuleHeightCheck = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + GetCapsuleComponent()->GetScaledCapsuleRadius() * 2;
			FVector upperGroundCheckForward = GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
			FVector upperGroundCheckStart = upperGroundCheckForward + GetTopLocation() + FVector::UpVector * capsuleHeightCheck;
			FVector upperGroundCheckEnd = upperGroundCheckForward + GetTopLocation() - FVector::DownVector * 10.0f/*for adjust*/;
			FHitResult upperGroundCheckHit = SphereTrace(upperGroundCheckStart, upperGroundCheckEnd, hangShimmySphereTraceRadius);
			DebugSphereTrace(upperGroundCheckHit, hangShimmySphereTraceRadius);

			FVector newTop = GetTopLocation();
			if (upperGroundCheckHit.bBlockingHit)
			{
				newTop.Z = upperGroundCheckHit.ImpactPoint.Z + hangShimmyHeightOffset;
			}

			// line trace to forward for forward check
			FVector forwardCheckStart = GetActorLocation() + FVector::UpVector * hangShimmyWallCheckHeight;
			FVector forwardCheckEnd = forwardCheckStart + GetActorForwardVector() * hangShimmyWallCheckDepth;
			FHitResult forwardCheckHit = LineTrace(forwardCheckStart, forwardCheckEnd);
			DebugLineTrace(forwardCheckHit);

			if (forwardCheckHit.bBlockingHit)
			{
				forwardCheckStart = GetActorLocation() + FVector::UpVector * hangShimmyWallCheckHeight;
				forwardCheckEnd = forwardCheckStart - forwardCheckHit.ImpactNormal * hangShimmyWallCheckDepth;
				forwardCheckHit = LineTrace(forwardCheckStart, forwardCheckEnd);
				DebugLineTrace(forwardCheckHit);

				if (FVector::Dist(forwardCheckStart, forwardCheckHit.ImpactPoint) > GetCapsuleComponent()->GetScaledCapsuleRadius() + hangShimmyWallSpareDistance/*for adjust*/)
				{
					FVector proj = FVector::PointPlaneProject(GetTopLocation(), forwardCheckHit.ImpactPoint, forwardCheckHit.ImpactNormal);
					proj += forwardCheckHit.ImpactNormal * GetCapsuleComponent()->GetScaledCapsuleRadius();
					newTop.X = proj.X;
					newTop.Y = proj.Y;
				}

				FVector wallNormalXY = -FVector(forwardCheckHit.ImpactNormal.X, forwardCheckHit.ImpactNormal.Y, 0);
				targetRotation = FRotationMatrix::MakeFromX(wallNormalXY).Rotator();

				if (forwardCheckHit.bBlockingHit)
				{
					// If moved to other component, reattach to other componenet
					USceneComponent* wallComp = forwardCheckHit.Component.Get();
					USceneComponent* parentComp = GetParentComponent();
					if (wallComp != parentComp)
					{
						// Attach to component
						FAttachmentTransformRules rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
						AttachToComponent(wallComp, rules);
					}
				}
			}

			SetTopLocation(newTop);

			// Add Movement input
			AddMovementInput(GetActorRightVector(), moveDirection.X);
		}
	}
}

void APlayerCharacter::ReleaseHang()
{
	// Sweep to down for floor check
	FVector floorCheckStart = GetFootLocation();
	FVector floorCheckEnd = floorCheckStart + FVector::DownVector * stepDownCheckDepth;
	FHitResult floorCheckHit = SphereTrace(floorCheckStart, floorCheckEnd, false);
	float floorAngle = FMathEx::BetweenAngle(floorCheckHit.ImpactNormal, FVector::UpVector);
	if (floorCheckHit.bBlockingHit && floorAngle <= GetCharacterMovement()->GetWalkableFloorAngle())
	{
		DebugSphereTrace(floorCheckHit, sphereTraceRadius, 3);

		SetCharacterState(ECharacterState::Default);

		MovementSlot& slot0 = movementSlots.FindOrAdd(0);

		// Set hit component
		slot0.hitComponent = floorCheckHit.Component;
		// Set world variables
		slot0.endLocalFootLocationOfComp = floorCheckHit.ImpactPoint;
		slot0.endLocalRotationOfComp = GetActorRotation().Quaternion();

		// Sweep to backward for move to backward on floor
		FVector floorBackwardCheckStart = floorCheckHit.ImpactPoint + FVector::UpVector * (sphereTraceRadius + sphereTraceFloorZOffset);
		FVector floorBackwardCheckEnd = floorBackwardCheckStart - GetActorForwardVector() * hangReleaseBackwardCheckLength;
		FHitResult floorBackwardCheckHit = SphereTrace(floorBackwardCheckStart, floorBackwardCheckEnd, sphereTraceRadius);
		DebugSphereTrace(floorBackwardCheckHit, sphereTraceRadius, 3);
		float backwardDistance = FMath::Clamp(floorBackwardCheckHit.Distance - GetCapsuleComponent()->GetScaledCapsuleRadius(), 0, MAX_FLT);
		slot0.endLocalFootLocationOfComp -= GetActorForwardVector() * backwardDistance;

		// Apply end transform variables
		slot0.endLocalFootLocationOfComp = slot0.hitComponent->GetComponentTransform().InverseTransformPosition(slot0.endLocalFootLocationOfComp);
		slot0.endLocalRotationOfComp = slot0.hitComponent->GetComponentTransform().InverseTransformRotation(slot0.endLocalRotationOfComp);

		// Play step down montage
		GetAnimInstanceEx()->Montage_Play(relaseGrabMontage);
	}
}

void APlayerCharacter::SetCharacterState(ECharacterState state)
{
	if (state == characterState)
		return;
	characterState = state;
	switch (characterState)
	{
		case ECharacterState::Default:
			RemoveFlyStack();
			GetCharacterMovement()->bCheatFlying = false;
			GetCapsuleComponent()->SetCapsuleRadius(42);
			DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true));
			rotatePerSec = prevRotatePerSec;
			break;
		case ECharacterState::Hang:
			AddFlyStack();
			GetCharacterMovement()->bCheatFlying = true;
			GetCapsuleComponent()->SetCapsuleRadius(39);
			prevRotatePerSec = rotatePerSec;
			rotatePerSec = hangShimmyRotatePerSec;
			break;
	}
	UThirdPersonAnim* anim = Cast<UThirdPersonAnim>(GetMesh()->GetAnimInstance());
	anim->SetCharacterState(characterState);
}

void APlayerCharacter::On_ANSTranslateGauge_NofityState(const FAnimNotifyStateData& data, const FAnimNotifyStateInfo& info)
{
	const FTranslateGaugeData& tgdata = Cast<UANSTranslateGauge>(data.AnimNotifyState)->GetData();
	MovementSlot* slot = movementSlots.Find(tgdata.Slot);

	// Save current position when begin notify
	if (data.Type == EAnimNotifyStateEventType::Begin)
	{
		slot->startFootLocation = GetFootLocation();
	}

	// Toggle movement mode
	if (tgdata.ToggleFlyDuringTranslate && characterState == ECharacterState::Default)
	{
		switch (data.Type)
		{
			case EAnimNotifyStateEventType::Begin:
			{
				AddFlyStack();
			}
			break;
			case EAnimNotifyStateEventType::End:
			{
				RemoveFlyStack();
			}
			break;
		}
	}

	FVector sv = slot->startFootLocation;
	FVector ev = slot->hitComponent->GetComponentTransform().TransformPosition(slot->endLocalFootLocationOfComp) + tgdata.Offset;

	FVector location;
	switch (tgdata.Type)
	{
		case ETranslateGaugeType::Linear:
		{
			location = FMath::Lerp(sv, ev, info.currentRatio);

			// Draw debug spline
			DrawDebugLine(GetWorld(), sv, ev, FColor::Cyan, false, 3);
		}
		break;
		case ETranslateGaugeType::Parabola:
		{
			FVector center = (sv + ev) * 0.5f + FVector::UpVector * tgdata.ParabolaOffset;
			location = FMathEx::BezierCurve(sv, center, ev, info.currentRatio);
			
			// Draw debug spline
			if (data.Type == EAnimNotifyStateEventType::Begin)
			{
				FVector prev = FMathEx::BezierCurve(sv, center, ev, 0);
				for (int i = 1; i <= 32; ++i)
				{
					float splineRatio = float(i) / float(32);
					FVector next = FMathEx::BezierCurve(sv, center, ev, splineRatio);
					DrawDebugLine(GetWorld(), prev, next, FColor::Cyan, false, 3);
					prev = next;
				}
			}
		}
		break;
	}
	SetFootLocation(location);
}

void APlayerCharacter::On_ANSRotateGauge_NofityState(const FAnimNotifyStateData& data, const FAnimNotifyStateInfo& info)
{
	const FRotateGaugeData& rgdata = Cast<UANSRotateGauge>(data.AnimNotifyState)->GetData();
	MovementSlot* slot = movementSlots.Find(rgdata.Slot);

	switch (data.Type)
	{
		case EAnimNotifyStateEventType::Begin:
			slot->startRotation = GetActorRotation().Quaternion();
			break;
	}

	FQuat sr = slot->startRotation;
	FQuat er = slot->hitComponent->GetComponentTransform().TransformRotation(slot->endLocalRotationOfComp);
	FQuat rotation = FQuat::Slerp(sr, er, info.currentRatio);
	SetRotationImmediate(rotation);
}

void APlayerCharacter::DefaultState_Nofity(const FAnimNotifyData& data)
{
	SetCharacterState(ECharacterState::Default);
}
