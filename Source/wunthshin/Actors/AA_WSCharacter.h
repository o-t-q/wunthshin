
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "wunthshin/Interfaces/Taker/Taker.h"
#include "Logging/LogMacros.h"

#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"
#include "wunthshin/Interfaces/ElementTracked/ElementTracked.h"

#include "AA_WSCharacter.generated.h"

class UC_WSShield;
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGlide);



DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game, Blueprintable)
class AA_WSCharacter : public ACharacter, public I_WSTaker, public IDataTableFetcher, public IElementTracked
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

	/** Zoom Wheel Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ZoomWheelAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* PickUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DropAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSInventory* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSShield* Shield;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UCharacterStatsComponent* CharacterStatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* RightHandWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	FName AssetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsFastRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCanGlide = false;

	// 빠르게 달리는 키가 눌려있는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsFastRunningPressing;

	// 걷는 키가 눌려있는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsWalkingPressing;

	// 앉기 키가 눌려있는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouchPressing;

public:
	//딜리게이트
	FFastRun OnFastRun;
	FUnFastRun OffFastRun;
	FOnWalk OnWalk;
	FOffWalk OffWalk;
	FTimerHandle FastRunCooldownTimer;
	FGlide Glide;
	
public:
	static const FName RightHandWeaponSocketName;
	static const float WalkCoolTime;
	
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

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsWalking() const { return bIsWalking; }

	// 커스텀 숙이기 가능 여부 함수
	bool CanBeCrouched() const;

	// 빠르게 달리기 가능 여부 함수
	bool CanFastRun() const;

	// 걷기 가능 여부 함수
	bool CanWalk() const;

	virtual UScriptStruct* GetTableType() const override;

	virtual void ApplyAsset(const FDataTableRowHandle& InRowHandle) override;

	virtual UClass* GetSubsystemType() const override;
#ifdef WITH_EDITOR
	virtual UClass* GetEditorSubsystemType() const override;
#endif

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void OnCrouch();
	void UnOnCrouch();

	void FastRun();
	void UnFastRun();

	void GoOnWalk();
	void GoOffWalk();

	void OnJump();
	void StopOnJump();

	void ZoomWheel(const FInputActionValue& Value);

	bool CanGlide();

	void FindAndTake();

	void CheckItemAndDrop();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;


	// 에셋 갱신
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	// 오른손 무기
	FORCEINLINE class UChildActorComponent* GetRightHandWeapon() const { return RightHandWeapon; }


	virtual bool Take(UC_WSPickUp* InTakenComponent) override;
	
	// 사용할 에셋의 이름
	void SetAssetName(const FName& InAssetName) { AssetName = InAssetName; }
};
