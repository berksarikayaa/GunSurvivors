

#include "Bullet.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);

	BulletSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BulletSprite"));
	BulletSprite->SetupAttachment(RootComponent);

	MovementDirection = FVector2D(1.0f, 0.0f);

}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLaunched)
	{
		FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;

		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, DistanceToMove.Y);

		SetActorLocation(NewLocation);
	}
}

void ABullet::Launch(FVector2D Direction, float Speed)
{
	if (IsLaunched) return;

	IsLaunched = true;

	MovementDirection = Direction;
	MovementSpeed = Speed;

	float DeleteTime = 10.0f;
	GetWorldTimerManager().SetTimer(DeleteTimer, this, &ABullet::OnDeleteTimerTimeout, 1.0f, false, DeleteTime);
}

void ABullet::OnDeleteTimerTimeout()
{
	Destroy();

}


