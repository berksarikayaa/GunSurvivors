

#include "EnemySpawner.h"
#include <Kismet/GameplayStatics.h>

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	AGameModeBase *GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (GameMode)
	{
		MyGameMode = Cast<AGunSurvivorsGameMode>(GameMode);
		check(MyGameMode);
	}

	AActor* PlayerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATopdownCharacter::StaticClass());
	if (PlayerActor)
	{
		Player = Cast<ATopdownCharacter>(PlayerActor);
		Player->PlayerDiedDelegate.AddDynamic(this, &AEnemySpawner::OnPlayerDied);
	}

	StartSpawning();
	
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::StartSpawning()
{
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::OnSpawnTimerTimeout, SpawnTime, true, SpawnTime);
}

void AEnemySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AEnemySpawner::OnSpawnTimerTimeout()
{
	SpawnEnemy();
}

void AEnemySpawner::SpawnEnemy()
{
	//Spawn enemy
	FVector2D RandomPosition = FVector2D(FMath::VRand());
	RandomPosition.Normalize();
	RandomPosition *= SpawnDistance;


	FVector EnemyLocation = GetActorLocation() + FVector(RandomPosition.X, 0.0f, RandomPosition.Y);

	AEnemy *Enemy = GetWorld()->SpawnActor<AEnemy>(EnemyActorToSpawn, EnemyLocation, FRotator::ZeroRotator);
	SetupEnemy(Enemy);

	//Increase the difficulty 
	TotalEnemyCount += 1;
	if((TotalEnemyCount % DifficultySpawnInterval)==0)
	{
		if (SpawnTime > SpawnTimeMinimumLimit)
		{
			SpawnTime -= DecreaseSpawnTimerByEveryInterval;
			if (SpawnTime < SpawnTimeMinimumLimit)
			{
				SpawnTime = SpawnTimeMinimumLimit;
			}
			StopSpawning();
			StartSpawning();
		}
	}
}

void AEnemySpawner::SetupEnemy(AEnemy* Enemy)
{
	if (Enemy)
	{
		Enemy->Player = Player;
		Enemy->CanFollow = true;
		Enemy->EnemyDiedDelegate.AddDynamic(this, &AEnemySpawner::OnEnemyDied);
	}
}

void AEnemySpawner::OnEnemyDied()
{

	int ScoreToAdd = 10;
	MyGameMode->AddScore(ScoreToAdd);
}

void AEnemySpawner::OnPlayerDied()
{
	StopSpawning();

	TArray<AActor*> EnemyArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), EnemyArray);
	for (AActor* EnemyActor : EnemyArray)
	{
		AEnemy* Enemy = Cast<AEnemy>(EnemyActor);
		if (Enemy && Enemy->IsAlive)
		{
			Enemy->CanFollow = false;
		}
	}

	 MyGameMode->RestartGame();
}


