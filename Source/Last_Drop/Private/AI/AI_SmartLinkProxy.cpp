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

float AAI_SmartLinkProxy::GetSmartLinkHeight() const
{
    const UNavLinkCustomComponent* SmartLink = GetSmartLinkComp();

    if (!SmartLink)
    {
        return 0.0f;
    }

    FVector Start;
    FVector End;
    ENavLinkDirection::Type Direction;

    SmartLink->GetLinkData(Start, End, Direction);

    return FMath::Abs(End.Z - Start.Z);
}

void AAI_SmartLinkProxy::SetSmartLinkPoints(FVector Start, FVector End)
{
    UNavLinkCustomComponent* SmartLink = GetSmartLinkComp();

    if (!SmartLink)
    {
        return;
    }

    FVector CurrentStart;
    FVector CurrentEnd;
    ENavLinkDirection::Type Direction;

    SmartLink->GetLinkData(CurrentStart, CurrentEnd, Direction);
    SmartLink->SetLinkData(Start, End, Direction);
}

