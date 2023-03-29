// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyEx.h"
#include "ANPlayMontage.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UANPlayMontage : public UAnimNotifyEx
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	UAnimMontage* montage = nullptr;

protected:
	virtual void OnNotify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
