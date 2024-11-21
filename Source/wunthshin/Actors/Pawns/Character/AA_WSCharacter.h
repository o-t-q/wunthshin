
#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

#include "wunthshin/Interfaces/Taker/Taker.h"
#include "wunthshin/Interfaces/DataTableFetcher/DataTableFetcher.h"
#include "wunthshin/Interfaces/ElementTracked/ElementTracked.h"
#include "wunthshin/Interfaces/CommonPawn/CommonPawn.h"
#include "wunthshin/Interfaces/SkillCast/SkillCast.h"

#include "AA_WSCharacter.generated.h"

class UC_WSCharacterInventory;
class UDamageCounterPool;
class UClimCharacterMovementComponent;
class UC_WSSkill;
class UPawnMovementComponent;
class UStatsComponent;
class UC_WSShield;
class AA_WSWeapon;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UC_WSInventory;
struct FInputActionValue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFastRun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnFastRun);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWalk);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOffWalk);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGlide);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game, Blueprintable)
class AA_WSCharacter : public ACharacter, public I_WSTaker, public IElementTracked, public ICommonPawn, public ISkillCast
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

	/** Clim Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ClimAction;

	/** Cancel Clim Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CancelClimAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* PickUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DropAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CharacterSwapOneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CharacterSwapTwoAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSCharacterInventory* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSShield* Shield;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UC_WSSkill* Skill;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	UStatsComponent* CharacterStatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* RightHandWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
	UDamageCounterPool* DamageCounters;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	FName AssetName;

	// 피격 애니메이션들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Asset", meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> HitMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsFastRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsGliding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsClimbing;

	// 빠르게 달리는 키가 눌려있는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsFastRunningPressing;

	// 걷는 키가 눌려있는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsWalkingPressing;

	// 앉기 키가 눌려있는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouchPressing;

	// 마지막으로 재생한 피격 애니메이션
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	int32 HitAnimationIndex;

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
	
	AA_WSCharacter(const FObjectInitializer& ObjectInitializer);

	// FastRun을 호출하는 공개 메서드
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void K2_FastRun();

	// UnFastRun을 호출하는 공개 메서드
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void K2_UnFastRun();

	// 커스텀 숙이기 가능 여부 함수
	bool CanBeCrouched() const;

	// 빠르게 달리기 가능 여부 함수
	bool CanFastRun() const;

	// 걷기 가능 여부 함수
	bool CanWalk() const;

	virtual UScriptStruct* GetTableType() const override;

	virtual void ApplyAsset(const FTableRowBase* InRowPointer) override;

	virtual UClass* GetSubsystemType() const override;
#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR
	virtual UClass* GetEditorSubsystemType() const override;
#endif

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void SwapCharacterOne();
	void SwapCharacterTwo();

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


	void Climb();
	void CancelClimb();
	

	

	

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;


	// 에셋 갱신
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual bool Take(UC_WSPickUp* InTakenComponent) override;
	
	// 사용할 에셋의 이름
	virtual void SetAssetName(const FName& InAssetName) override { AssetName = InAssetName; }

	// ICommonPawn
	virtual FName                   GetAssetName() const override { return AssetName; }
	virtual UCapsuleComponent*      GetCapsuleComponent() const override { return ACharacter::GetCapsuleComponent(); }
	virtual USkeletalMeshComponent* GetSkeletalMeshComponent() const override { return GetMesh(); }
	virtual IInventoryComponent*    GetInventoryComponent() const override;
	virtual UC_WSShield*            GetShieldComponent() const override { return Shield; }
	virtual UStatsComponent*        GetStatsComponent() const override { return CharacterStatsComponent; }
	virtual UChildActorComponent*   GetRightHandComponent() const override { return RightHandWeapon; }
	virtual UPawnMovementComponent* GetPawnMovementComponent() const override { return ACharacter::GetMovementComponent(); }
	virtual UC_WSSkill*             GetSkillComponent() const override { return Skill; }
	virtual UDamageCounterPool*     GetDamageCounters() const override { return DamageCounters; }
	
	virtual void HandleStaminaDepleted() override;
	UInputMappingContext* GetMappingContext() const { return DefaultMappingContext; }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool IsFastRunning() const override { return bIsFastRunning; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool IsWalking() const override { return bIsWalking; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool CheckClimbState() const override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool CheckClimbDashState() const override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual FVector ClimbDashDirection() const override;
	

private:
	virtual void SetHitMontages(const TArray<UAnimMontage*>& InMontages) override;

public:
	virtual void PlayHitMontage() override;
	
protected:
	UPROPERTY(Category=Character , VisibleAnywhere,BlueprintReadOnly)
	UClimCharacterMovementComponent* CilmMovementComponent;

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE UClimCharacterMovementComponent* GetCustomCharacterMovement() const { return CilmMovementComponent; }
};
