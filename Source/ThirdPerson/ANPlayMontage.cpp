// Fill out your copyright notice in the Description page of Project Settings.


#include "ANPlayMontage.h"

void UANPlayMontage::OnNotify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::OnNotify(MeshComp, Animation, EventReference);

	if (!montage)
		return;

	UAnimInstance* anim = MeshComp->GetAnimInstance();
	if (anim)
	{
		anim->Montage_Play(montage);
	}
}
