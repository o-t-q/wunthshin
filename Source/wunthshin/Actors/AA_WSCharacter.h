
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "wunthshin/Interfaces/Taker/Taker.h"
#include "Logging/LogMacros.h"

#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"

#include "AA_WSCharacter.generated.h"

class AA_WSWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UC_WSInventory;
class UCharacterStatsComponent;
struct FInputActionValue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFastRun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnFastRun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWalk);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOffWalk);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game, Blueprintable)
class AA_WSCharacter : public ACharacter, public I_WSTaker, public IDataTableFetcher
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Crouch Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/** FastRun Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FastRunAction;

	/** Walk Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WalkAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* PickUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DropAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Test", meta = (AllowPrivateAccess = "true"))
	UC_WSInventory* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UCharacterStatsComponent* CharacterStatsComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	bool bIsFastRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* RightHandWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	FName AssetName;

public:
	FFastRun OnFastRun;
	FUnFastRun OffFastRun;
	FOnWalk OnWalk;
	FOffWalk OffWalk;
	FTimerHandle FastRunCooldownTimer;

public:
	static const FName RightHandWeaponSocketName;
	
	AA_WSCharacter();

	// 스태미나가 0일 때 호출하는 공개 메서드
	void HandleStaminaDepleted();

	// FastRun을 호출하는 공개 메서드
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void K2_FastRun();

	// UnFastRun을 호출하는 공개 메서드
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void K2_UnFastRun();

	// FastRun 상태를 반환하는 함수
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsFastRunning() const { return bIsFastRunning; }

	virtual UScriptStruct* GetTableType() const override;

	virtual void ApplyAsset(const FDataTableRowHandle& InRowHandle) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void OnCrouch(const FInputActionValue& Value);
	void UnOnCrouch(const FInputActionValue& Value);

	void FastRun(const FInputActionValue& Value);
	void UnFastRun(const FInputActionValue& Value);

	void GoOnWalk(const FInputActionValue& Value);
	void GoOffWalk(const FInputActionValue& Value);

	void FindAndTake();

	void CheckItemAndDrop();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	// 에셋 갱신
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual bool Take(UC_WSPickUp* InTakenComponent) override;
	
	// 사용할 에셋의 이름
	void SetAssetName(const FName& InAssetName) { AssetName = InAssetName; }
};
