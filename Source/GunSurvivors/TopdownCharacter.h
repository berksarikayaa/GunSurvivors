
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "PaperFlipbookComponent.h"
#include "PaperSpriteComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Controller.h"
#include "Engine/TimerHandle.h"
#include "Sound/SoundBase.h"
#include "Bullet.h"

#include "TopdownCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDiedDelegate);

UCLASS()
class GUNSURVIVORS_API ATopdownCharacter : public APawn
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperFlipbookComponent* CharacterFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperSpriteComponent* GunSprite;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* GunParent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* BulletSpawnPosition;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperFlipbook* IdleFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperFlipbook* RunFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* BulletShootSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* DieSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D HorizontalLimits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VerticalLimits;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABullet> BulletActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 100.0f;

	UPROPERTY(BlueprintReadWrite)
	FVector2D MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanShoot = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShootCooldowndDurationInSeconds = 0.3f;

	FTimerHandle ShootCooldownTimer;

	FPlayerDiedDelegate PlayerDiedDelegate;

	ATopdownCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveTriggered(const FInputActionValue& Value);
	void MoveCompleted(const FInputActionValue& Value);

	void Shoot(const FInputActionValue& Value);

	bool IsInMapBoundsHorizontal(float XPos);
	bool IsInMapBoundsVertical(float ZPos);

	void OnShootCooldownTimerTimeout();

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

