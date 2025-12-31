// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

void ATower::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle FireTimerHandle;
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

ATower::ATower()
{

}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InFireRange()) { RotateTurret(Tank->GetActorLocation()); }
}

void ATower::CheckFireCondition()
{
	if (Tank && Tank->IsAlive && InFireRange()) { Fire(); }
}

bool ATower::InFireRange()
{
	if (Tank)
	{
		if (FVector::Distance(GetActorLocation(), Tank->GetActorLocation()) <= ActivateDistance)
		{
			return true;
		}
	}
	return false;
}

void ATower::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}
