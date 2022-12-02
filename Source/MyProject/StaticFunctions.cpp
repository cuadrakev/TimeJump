// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticFunctions.h"

void UStaticFunctions::OrderByDistance(AActor* PlayerActor, TArray<AActor*> inArray, TArray<AActor*>& outArray)
{
	inArray.Sort([PlayerActor](const AActor& A, const AActor& B) {
		float distA = A.GetDistanceTo(PlayerActor);
		float distB = B.GetDistanceTo(PlayerActor);
		return distA > distB;
		}
	);
	outArray = inArray;
}