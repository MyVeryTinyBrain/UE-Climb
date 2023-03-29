// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyStateEx.h"

UAnimNotifyStateEx::UAnimNotifyStateEx()
{
	bIsNativeBranchingPoint = true;
	bShouldFireInEditor = false;
}

FAnimNotifyStateInfo UAnimNotifyStateEx::GetAnimNotifyStateInfo(const FAnimNotifyStateData& data) const
{
	FAnimNotifyStateInfo info;
	info.isValid = false;

	// Fail if data is other anim notify state's data
	if (this != data.AnimNotifyState)
		return info;

	// Find notify event from current animation
	FAnimNotifyEvent* notifyEvent = nullptr;
	for (auto& currentNotifyEvent : data.Animation->Notifies)
	{
		if (currentNotifyEvent.NotifyStateClass.Get() == data.AnimNotifyState)
		{
			notifyEvent = &currentNotifyEvent;
			break;
		}
	}

	if (!notifyEvent)
		return info;

	// Information is valid from this line on
	info.isValid = true;

	// Initialize montage only variables
	info.isInMontage = false;
	info.currentTime = -1;
	info.currentRatio = -1;

	// Get times
	info.duration = notifyEvent->GetDuration();
	info.start = notifyEvent->GetTriggerTime();
	info.end = info.start + info.duration;
	
	if (UAnimMontage* montage = Cast<UAnimMontage>(data.Animation))
	{
		// Find montage instance from anim instance
		// It's using for GetPosition()
		FAnimMontageInstance* montageInstance = nullptr;
		for (FAnimMontageInstance* currentMontageInstance : data.MeshComp->GetAnimInstance()->MontageInstances)
		{
			if (currentMontageInstance->Montage == montage)
			{
				montageInstance = currentMontageInstance;
				break;
			}
		}

		// Just return if anim notify state is not in animation montage
		if (!montageInstance)
			return info;

		// Calculate ratio
		float time = montageInstance->GetPosition();
		float ratio = 0;
		if (time < info.start || data.Type == EAnimNotifyStateEventType::Begin)
			ratio = 0;
		else if (time > info.end || data.Type == EAnimNotifyStateEventType::End)
			ratio = 1;
		else
			ratio = (time - info.start) / info.duration;

		info.isInMontage = true;
		info.currentTime = time;
		info.currentRatio = ratio;
	}

	return info;
}

bool UAnimNotifyStateEx::CanNotifyInEditorTool(USkeletalMeshComponent* MeshComp) const
{
	if (!bShouldFireInEditor)
	{
		if (!MeshComp->GetWorld())
			return true;
		if (MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview)
			return true;
	}
	return false;
}

void UAnimNotifyStateEx::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (CanNotifyInEditorTool(MeshComp))
		return;

	OnNotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UAnimNotifyStateEx::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (CanNotifyInEditorTool(MeshComp))
		return;

	OnNotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyStateEx::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (CanNotifyInEditorTool(MeshComp))
		return;

	OnNotifyEnd(MeshComp, Animation, EventReference);
}

void UAnimNotifyStateEx::OnNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	CallAnimNotifyStateCallback(MeshComp->GetAnimInstance(), EAnimNotifyStateEventType::Begin, MeshComp, Animation, EventReference, TotalDuration);
	CallAnimNotifyStateCallback(MeshComp->GetOwner(), EAnimNotifyStateEventType::Begin, MeshComp, Animation, EventReference, TotalDuration);
}

void UAnimNotifyStateEx::OnNotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	CallAnimNotifyStateCallback(MeshComp->GetAnimInstance(), EAnimNotifyStateEventType::Tick, MeshComp, Animation, EventReference, 0, FrameDeltaTime);
	CallAnimNotifyStateCallback(MeshComp->GetOwner(), EAnimNotifyStateEventType::Tick, MeshComp, Animation, EventReference, 0, FrameDeltaTime);
}

void UAnimNotifyStateEx::OnNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	CallAnimNotifyStateCallback(MeshComp->GetAnimInstance(), EAnimNotifyStateEventType::End, MeshComp, Animation, EventReference);
	CallAnimNotifyStateCallback(MeshComp->GetOwner(), EAnimNotifyStateEventType::End, MeshComp, Animation, EventReference);
}

void UAnimNotifyStateEx::CallAnimNotifyStateCallback(UObject* Object, EAnimNotifyStateEventType Type, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference, float TotalDuration, float FrameDeltaTime) const
{
	if (!Object)
		return;

	UFunction* function = Object->FindFunction(*GetCallbackFunctionName());
	if (!function)
		return;

	if (function->NumParms != 1)
		return;

	FAnimNotifyStateData data;
	data.AnimNotifyState = this;
	data.Type = Type;

	data.MeshComp = MeshComp;
	data.Animation = Animation;
	data.EventReference = &EventReference;
	data.TotalDuration = TotalDuration;
	data.FrameDeltaTime = FrameDeltaTime;

	FAnimNotifyStateCallback Delegate;
	Delegate.BindUFunction(Object, *GetCallbackFunctionName());
	Delegate.Execute(data);
}
