// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "BattleBlasterGameInstance.h"

void ABattleBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);
	TowerCount = Towers.Num();

	UE_LOG(LogTemp, Display, TEXT("Number of towers from mutated Towers array from GetAllActorsOfClass: %d"), TowerCount);
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{

		this->Tank = Cast<ATank>(PlayerPawn);
		if (Tank)
		{
			int32 LoopIndex = 0;
			while (LoopIndex < TowerCount)
			{
				if (ATower* Tower = Cast<ATower>(Towers[LoopIndex++]))
				{
					Tower->Tank = this->Tank;
					UE_LOG(LogTemp, Display, TEXT("Tower number %d just assigned player tank as its target!"), LoopIndex);
				}
			}
		}
	}

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		ScreenMessageWidget = CreateWidget<UScreenMessage>(PlayerController, ScreenMessageClass);
		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->AddToPlayerScreen();
			ScreenMessageWidget->SetMessageText("Get Ready!");
		}
	}

	CountdownSeconds = CountdownDelay;
	//GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ABattleBlasterGameMode::OnCountdownTimerTimeout(), 1.0f, CountdownSeconds > 0 ? true : false);
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ABattleBlasterGameMode::OnCountdownTimerTimeout, 1.0f, true);
}

void ABattleBlasterGameMode::OnCountdownTimerTimeout()
{
	if (CountdownSeconds > 0)
	{ 
		//UE_LOG(LogTemp, Display, TEXT("%d..."), CountdownSeconds--); 
		FString CountdownText = FString::Printf(TEXT("%d..."), CountdownSeconds--);
		ScreenMessageWidget->SetMessageText(CountdownText);
	}
	else if (CountdownSeconds == 0)
	{ 
		//UE_LOG(LogTemp, Display, TEXT("Go...!"), CountdownSeconds);
		ScreenMessageWidget->SetMessageText("Go!");
		CountdownSeconds--;
		Tank->SetPlayerEnabled(true);
	}
	else
	{
		//ScreenMessageWidget->SetMessageText("");
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	}
}

void ABattleBlasterGameMode::ActorDied(AActor* DeadActor)
{
	bool IsGameOver = false;
	if (DeadActor == Tank)
	{
		// Tank just died
		//UE_LOG(LogTemp, Display, TEXT("Tank died, defeated!"));
		Tank->HandleDestruction();
		IsGameOver = true;
	}
	else
	{
		// A Tower just died
		//UE_LOG(LogTemp, Display, TEXT("Tower died, fk that mofo!"));
		if (ATower* DeadTower = Cast<ATower>(DeadActor))
		{
			DeadTower->HandleDestruction();
			//UE_LOG(LogTemp, Display, TEXT("Tower died, fk that mofo!"));
			if (--TowerCount <= 0)
			{
				IsVictorious = true;
				IsGameOver = true;
				//UE_LOG(LogTemp, Display, TEXT("That was the last tower, great job! You win!"));
			}
		}
	}

	if (IsGameOver)
	{
		FString GameOverString = IsVictorious ? "Victory!" : "Defeat!";
		//UE_LOG(LogTemp, Display, TEXT("%s"), *GameOverString);
		ScreenMessageWidget->SetMessageText(GameOverString);
		ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);


		FTimerHandle GameOverTimerHandle;
		GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ABattleBlasterGameMode::OnGameOverTimeout, GameOverDelay, false);
	}
}

void ABattleBlasterGameMode::OnGameOverTimeout()
{
	//UE_LOG(LogTemp, Display, TEXT("Fired delay?"));
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UBattleBlasterGameInstance* BattleBlasterGameInstance = Cast<UBattleBlasterGameInstance>(GameInstance))
		{
			if (IsVictorious)
			{
				// Load next level
				BattleBlasterGameInstance->LoadNextLevel();
			}
			else
			{
				// Reload current level
				BattleBlasterGameInstance->RestartCurrentLevel();

			}
		}
	}
	
	//UGameplayStatics::OpenLevel(GetWorld(), *CurrentLevelName);
}
