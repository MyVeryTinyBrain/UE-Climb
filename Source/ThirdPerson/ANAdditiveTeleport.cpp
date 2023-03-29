// Fill out your copyright notice in the Description page of Project Settings.


#include "ANAdditiveTeleport.h"

void UANAdditiveTeleport::OnNotify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::OnNotify(MeshComp, Animation, EventReference);

	AActor* actor = MeshComp->GetOwner();
	if (!actor)
		return;
	
	actor->SetActorLocation(actor->GetActorLocation() + additiveVector, sweep);
}
