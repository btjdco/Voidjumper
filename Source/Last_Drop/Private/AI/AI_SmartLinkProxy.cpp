// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AI_SmartLinkProxy.h"
#include "NavLinkCustomComponent.h"

void AAI_SmartLinkProxy::GetSmartLinkPoints(FVector& Start, FVector& End) const
{
    Start = FVector::ZeroVector;
    End = FVector::ZeroVector;

    if (const UNavLinkCustomComponent* SmartLink = GetSmartLinkComp())
    {
        ENavLinkDirection::Type Direction;
        SmartLink->GetLinkData(Start, End, Direction);
    }
}
