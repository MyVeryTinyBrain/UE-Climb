// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyEx.h"
#include "ANAdditiveTeleport.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UANAdditiveTeleport : public UAnimNotifyEx
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FVector additiveVector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	bool sweep = false;

protected:
	virtual void OnNotify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
