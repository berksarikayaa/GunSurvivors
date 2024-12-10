

#include "TopdownCharacter.h"

#include "Kismet/KismetMathLibrary.h"

ATopdownCharacter::ATopdownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	CharacterFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("CharacterFlipbook"));
	CharacterFlipbook->SetupAttachment(RootComponent);

	GunParent = CreateDefaultSubobject<USceneComponent>(TEXT("GunParent"));
	GunParent->SetupAttachment(RootComponent);

	GunSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("GunSprite"));
	GunSprite->SetupAttachment(GunParent);

	BulletSpawnPosition = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawnPosition"));
	BulletSpawnPosition->SetupAttachment(GunSprite);

}

void ATopdownCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);

		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	
}

bool ATopdownCharacter::IsInMapBoundsHorizontal(float XPos)
{
	bool Result = true;

	Result = (XPos > HorizontalLimits.X) && (XPos < HorizontalLimits.Y);

	return Result;

}

bool ATopdownCharacter::IsInMapBoundsVertical(float ZPos)
{

	bool Result = true;

	Result = (ZPos > VerticalLimits.X) && (ZPos < VerticalLimits.Y);

	return Result;

}




void ATopdownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Move Player
	if(CanMove)
	{
		if (MovementDirection.Length() > 0.0f)
		{
			if (MovementDirection.Length() > 1.0f)
			{
				MovementDirection.Normalize();
			}
			FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;

			FVector CurrentLocation = GetActorLocation();
			FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, 0.0f);
			if (!IsInMapBoundsHorizontal(NewLocation.X))
			{
				NewLocation -= FVector(DistanceToMove.X, 0.0f, 0.0f);
			}

			NewLocation += FVector(0.0f, 0.0f, DistanceToMove.Y);
			if (!IsInMapBoundsVertical(NewLocation.Z))
			{
				NewLocation -= FVector(0.0f, 0.0f, DistanceToMove.Y);
			}

			SetActorLocation(NewLocation);
		}
	}

	//Rotate Gun

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		FVector MouseWorldLocation, MouseWorldDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);


		FVector CurrentLocation = GetActorLocation();
		FVector Start = FVector(CurrentLocation.X, 0.0f, CurrentLocation.Z);
		FVector Target = FVector(MouseWorldLocation.X, 0.0f, MouseWorldLocation.Z);
		FRotator GunParentRotator = UKismetMathLibrary::FindLookAtRotation(Start, Target);

		GunParent->SetRelativeRotation(GunParentRotator);
	}


}

void ATopdownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopdownCharacter::MoveTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATopdownCharacter::MoveCompleted);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ATopdownCharacter::MoveCompleted);

		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATopdownCharacter::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATopdownCharacter::Shoot);

	}


}

void ATopdownCharacter::MoveTriggered(const FInputActionValue& Value)
{
	FVector2D MoveActionValue = Value.Get<FVector2D>();

	if (CanMove)
	{
		MovementDirection = MoveActionValue;
		CharacterFlipbook->SetFlipbook(RunFlipbook);

		FVector FlipbookScale = CharacterFlipbook->GetComponentScale();
		if (MovementDirection.X < 0.0f)
		{
			if(FlipbookScale.X > 0.0f)
			{
				CharacterFlipbook->SetWorldScale3D(FVector(-1.0f, 1.0f, 1.0f));
			}
		}
		else if (MovementDirection.X > 0.0f)
		{
			if (FlipbookScale.X < 0.0f)
			{
				CharacterFlipbook->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
			}
		}

	}
}

void ATopdownCharacter::MoveCompleted(const FInputActionValue& Value)
{
	MovementDirection = FVector2D(0.0f, 0.0f);
	CharacterFlipbook->SetFlipbook(IdleFlipbook);

}

void ATopdownCharacter::Shoot(const FInputActionValue& Value)
{
	if (CanShoot)
	{
		CanShoot = false;

		//Spawn Bullet Actor
		ABullet *Bullet = GetWorld()->SpawnActor<ABullet>(BulletActorToSpawn, BulletSpawnPosition->GetComponentLocation(), 
			FRotator(0.0f, 0.0f, 0.0f));
		check(Bullet);

		//Get Mouse World Location
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		check(PlayerController);
		FVector MouseWorldLocation, MouseWorldDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

		//Calculate Bullet Direction
		FVector CurrentLocation = GetActorLocation();
		FVector2D BulletDirection = FVector2D(MouseWorldLocation.X - CurrentLocation.X, MouseWorldLocation.Z - CurrentLocation.Z);
		BulletDirection.Normalize();

		//Launch The Bullet
		float BulletSpeed = 300.0f;
		Bullet->Launch(BulletDirection, BulletSpeed);



		GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &ATopdownCharacter::OnShootCooldownTimerTimeout, 1.0f, false, ShootCooldowndDurationInSeconds);
	}
}

void ATopdownCharacter::OnShootCooldownTimerTimeout()
{
	CanShoot = true;
}


