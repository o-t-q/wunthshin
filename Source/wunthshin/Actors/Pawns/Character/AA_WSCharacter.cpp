
#include "AA_WSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/NavMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "wunthshin/Actors/Item/A_WSItem.h"
#include "wunthshin/Actors/Item/Weapon/A_WSWeapon.h"
#include "InputMappingContext.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "wunthshin/Enums.h"
#include "wunthshin/Components/Shield/C_WSShield.h"
#include "wunthshin/Data/Characters/CharacterTableRow/CharacterTableRow.h"
#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"
#include "wunthshin/Components/ClimCharacterMovementComponent.h"

#include "wunthshin/Components/Stats/StatsComponent.h"
#include "wunthshin/Data/Items/DamageEvent/WSDamageEvent.h"
#include "wunthshin/Subsystem/Utility.h"
#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR
#include "wunthshinEditorModule/Subsystem/EditorSubsystem/Character/CharacterEditorSubsystem.h"
#endif
#include "wunthshin/Components/CharacterInventory/C_WSCharacterInventory.h"
#include "wunthshin/Components/Skill/C_WSSkill.h"
#include "wunthshin/Subsystem/GameInstanceSubsystem/Character/CharacterSubsystem.h"
#include "wunthshin/Subsystem/WorldSubsystem/WorldStatus/WorldStatusSubsystem.h"
#include "wunthshin/Widgets/DamageCounter/WG_WSDamageCounter.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

#define ADD_INPUT_ACTION(VariableName, Path)                                                    \
    static ConstructorHelpers::FObjectFinder<UInputAction> IA_##VariableName(TEXT(Path));       \
    if ((IA_##VariableName).Succeeded())                                                         \
    {                                                                                           \
        VariableName = (IA_##VariableName).Object;                                              \
    }

//////////////////////////////////////////////////////////////////////////
// AwunthshinCharacter

const FName AA_WSCharacter::RightHandWeaponSocketName = TEXT("WeaponProp02");
const float AA_WSCharacter::WalkCoolTime = 0.2f;

AA_WSCharacter::AA_WSCharacter(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UClimCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    {
        static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_Default(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ThirdPerson/Input/IMC_Default.IMC_Default'"));
        
        if (IMC_Default.Succeeded()) 
        {
            DefaultMappingContext = IMC_Default.Object;
        }

        ADD_INPUT_ACTION(JumpAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump'");
        ADD_INPUT_ACTION(MoveAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move'");
        ADD_INPUT_ACTION(CrouchAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Crouch.IA_Crouch'");
        ADD_INPUT_ACTION(FastRunAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_FastRun.IA_FastRun'");
        ADD_INPUT_ACTION(WalkAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Walk.IA_Walk'");
        ADD_INPUT_ACTION(LookAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look'");
        ADD_INPUT_ACTION(PickUpAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_PickUp.IA_PickUp'");
        ADD_INPUT_ACTION(DropAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Drop.IA_Drop'");
        ADD_INPUT_ACTION(ZoomWheelAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_ZoomWheel.IA_ZoomWheel'");
        ADD_INPUT_ACTION(ClimAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Climb.IA_Climb'");
        ADD_INPUT_ACTION(CancelClimAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_CancelClimb.IA_CancelClimb'");
        ADD_INPUT_ACTION(CharacterSwapOneAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Character1.IA_Character1'");
        ADD_INPUT_ACTION(CharacterSwapTwoAction, "/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Input/Actions/IA_Character2.IA_Character2'")
    }


	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // 매시를 캡슐 콜리전 원점으로 옮김
    GetMesh()->SetRelativeLocation({ 0.f, 0.f, -96.f });
    GetMesh()->SetRelativeRotation({ 0.f, 270.f, 0.f });

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...   
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
    GetCharacterMovement()->GravityScale = 1.0f;
    GetCharacterMovement()->SetWalkableFloorAngle(50.f);

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character   
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Create a Inventory
    Inventory = CreateDefaultSubobject<UC_WSCharacterInventory>(InventoryComponentName);
    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

    // Create CharacterStats
    CharacterStatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("CharacterStatsComponent"));

    // Create a shield component.
    Shield = CreateDefaultSubobject<UC_WSShield>(TEXT("ShieldComponent"));
    Shield->SetupAttachment(GetMesh());
    
    RightHandWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightHandWeapon"));
    
	CameraBoom->bEnableCameraLag = true;

    CilmMovementComponent = Cast<UClimCharacterMovementComponent>(GetCharacterMovement());

    Skill = CreateDefaultSubobject<UC_WSSkill>(TEXT("SkillComponent"));
    
    DamageCounters = CreateDefaultSubobject<UDamageCounterPool>(TEXT("DamageIndicators"));

    AutoPossessPlayer = EAutoReceiveInput::Type::Disabled;
    AutoPossessAI = EAutoPossessAI::Disabled;
}

void AA_WSCharacter::HandleStaminaDepleted()
{
    // protected UnFastRun을 CharacterStatsComponent에서 호출하기 위함.
    UnFastRun();
}

bool AA_WSCharacter::CheckClimbState() const
{
    return CilmMovementComponent->bWantsToClimbVeiw();
}

bool AA_WSCharacter::CheckClimbDashState() const
{
    return CilmMovementComponent->IsClimbDashing();
}

FVector AA_WSCharacter::ClimbDashDirection() const
{
    return CilmMovementComponent->GetClimbDashDirection();
}

void AA_WSCharacter::SetHitMontages(const TArray<UAnimMontage*>& InMontages)
{
    HitMontages = InMontages;
}

void AA_WSCharacter::PlayHitMontage()
{
    if (HitMontages.IsEmpty())
    {
        return;
    }
    
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        if (UAnimMontage* AnimMontage = HitMontages[HitAnimationIndex])
        {
            AnimInstance->Montage_Play(AnimMontage);
            HitAnimationIndex++;
            HitAnimationIndex %= HitMontages.Num();
        }
    }
}

void AA_WSCharacter::K2_FastRun()
{
    FastRun();
}

void AA_WSCharacter::K2_UnFastRun()
{
    UnFastRun();
}

bool AA_WSCharacter::CanBeCrouched() const
{
    return !GetMovementComponent()->IsFalling() &&
        !GetCharacterMovement()->bWantsToCrouch &&
        !bIsFastRunning;
}

bool AA_WSCharacter::CanFastRun() const
{
    return !GetCharacterMovement()->bWantsToCrouch &&  // 이전 틱 + 현재 틱을 포함하여 Crouch중인 상황 (Crouch 업데이트가 lazy하게 발생함)
        !bIsFastRunning &&
        !bIsWalking;
}

bool AA_WSCharacter::CanWalk() const
{
    return !bIsWalking && !bIsFastRunning;
}

UScriptStruct* AA_WSCharacter::GetTableType() const
{
    return FCharacterTableRow::StaticStruct();
}

void AA_WSCharacter::ApplyAsset(const FTableRowBase* InRowPointer)
{
    if (!InRowPointer) return;

    const FCharacterTableRow* Data = reinterpret_cast<const FCharacterTableRow*>(InRowPointer);
    
    UpdatePawnFromDataTable(Data);
    
    if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
    {
        if (const UStatsComponent* StatsComponent = GetStatsComponent())
        {
            const FCharacterMovementStats& MovementStats = StatsComponent->GetMovementStats();
            MovementComponent->MaxWalkSpeed = MovementStats.GetNormalMaxSpeed();
            MovementComponent->MaxFlySpeed = MovementStats.GetFlyingMaxSpeed();
            MovementComponent->MaxWalkSpeedCrouched = MovementStats.GetCrouchMaxSpeed();
            MovementComponent->JumpZVelocity = MovementStats.GetInitialJumpVelocity();
        }
    }
}

UClass* AA_WSCharacter::GetSubsystemType() const
{
    return UCharacterSubsystem::StaticClass();
}

#if WITH_EDITOR & !UE_BUILD_SHIPPING_WITH_EDITOR
UClass* AA_WSCharacter::GetEditorSubsystemType() const
{
    return UCharacterEditorSubsystem::StaticClass();
}
#endif

void AA_WSCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();
    
    BLUEPRINT_REFRESH_EDITOR

    // 무기를 소환하는 차일드 액터 컴포넌트가 매시에 제대로 부착되었는지 확인
    ensure(
        RightHandWeapon->AttachToComponent
        (
            GetMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            RightHandWeaponSocketName
        ));
}

void AA_WSCharacter::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    FetchAsset(AssetName);
}

float AA_WSCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (FWSDamageEvent const& CustomEvent = reinterpret_cast<FWSDamageEvent const&>(DamageEvent);
        CustomEvent.IsFirstHit(this))
    {
        if (CharacterStatsComponent->GetHP() > 0)
        {
            CharacterStatsComponent->DecreaseHP(Damage);
            UE_LOG(LogTemplateCharacter, Warning, TEXT("TakeDamage! : %s did %f with %s to %s"), *EventInstigator->GetName(), Damage, *DamageCauser->GetName(), *GetName());
            CustomEvent.SetFirstHit(this);
            PlayHitMontage();
            DamageCounters->Allocate(Damage);

            // 무기를 맞았을 경우 무기의 원소 효과를 부여
            if (const AA_WSWeapon* Weapon = Cast<AA_WSWeapon>(DamageCauser))
            {
                ApplyElement(EventInstigator, Weapon->GetElement());
            }

            Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
            return Damage;
        }
    }

    return 0.f;
}

void AA_WSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsGliding)
    {
        if (!GetCharacterMovement()->IsFalling())
        {
            bIsGliding = false;
            StopJumping();
        }
        else
        {
            //GetCharacterMovement()->GravityScale = 0.05f;
            GetCharacterMovement()->Velocity.Z = FMath::Clamp(GetCharacterMovement()->Velocity.Z, -100.f, 0.0f);   
        }
    }
}

bool AA_WSCharacter::Take(UC_WSPickUp* InTakenComponent)
{
    AA_WSItem* Item = Cast<AA_WSItem>(InTakenComponent->GetOwner());
    ensure(Item);
    UE_LOG(LogTemplateCharacter, Log, TEXT("Pick up item: %s"), *Item->GetName());

    // 손이 비어있고, 무기를 잡으려 할때
    if (const AA_WSWeapon* WeaponCast = Cast<AA_WSWeapon>(Item);
        WeaponCast && !RightHandWeapon->GetChildActor())
    {
        RightHandWeapon->SetChildActorClass(WeaponCast->GetClass());
        
        // 런타임 에셋 설정을 위해 Deferred spawn이 필요함
        RightHandWeapon->CreateChildActor([Item](AActor* InActor) 
            {
                AA_WSItem* GeneratedItem = Cast<AA_WSItem>(InActor);
                // 생성에 사용된 클래스가 Item 상속 클래스가 아닌경우
                check(GeneratedItem);

                GeneratedItem->SetAssetName(Item->GetAssetName());
            });

        // 손에 있는 무기를 주울 수 없도록 pick up component를 비활성화
        if (UC_WSPickUp* PickUpComponent = RightHandWeapon->GetChildActor()->GetComponentByClass<UC_WSPickUp>()) 
        {
            PickUpComponent->SetTaken(this);
        }

        // 충돌 반응 비활성화, overlap으로 반응하는 아이템 프로필로 설정
        RightHandWeapon->GetChildActor()->GetComponentByClass<UMeshComponent>()->SetCollisionProfileName("ItemEquipped");
    }
    
    // 인벤토리로 무기 또는 아이템 저장
    Inventory->AddItem(Item);

    // todo/test: 아이템 효과 테스트, 이후에 삭제 필요함
    // {
    //     if (!Item->IsA<AA_WSWeapon>()) 
    //     {
    //         int32 Index = Inventory->FindItemIndex(Item->GetItemMetadata());
    //
    //         if (Index != INDEX_NONE)
    //         {
    //             Inventory->UseItem(Index, this);
    //         }
    //     }
    // }
    
    return true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AA_WSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AA_WSCharacter::OnJump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AA_WSCharacter::StopOnJump);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AA_WSCharacter::Move);

		// Cruuch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AA_WSCharacter::OnCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AA_WSCharacter::UnOnCrouch);

		// FastRun
		EnhancedInputComponent->BindAction(FastRunAction, ETriggerEvent::Started, this, &AA_WSCharacter::FastRun);
		EnhancedInputComponent->BindAction(FastRunAction, ETriggerEvent::Completed, this, &AA_WSCharacter::UnFastRun);

		// Walk
        EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &AA_WSCharacter::GoOnWalk);
        EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &AA_WSCharacter::GoOffWalk);

        // ZoomWheel
        EnhancedInputComponent->BindAction(ZoomWheelAction, ETriggerEvent::Started, this, &AA_WSCharacter::ZoomWheel);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AA_WSCharacter::Look);

        // 줍기 (Started로 한번 눌렸을 때만 실행되도록 처리)
        EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &AA_WSCharacter::FindAndTake);

        // 떨어뜨리기
        EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &AA_WSCharacter::CheckItemAndDrop);

        // 등반
        EnhancedInputComponent->BindAction(ClimAction, ETriggerEvent::Triggered, this, &AA_WSCharacter::Climb);

        EnhancedInputComponent->BindAction(CancelClimAction, ETriggerEvent::Started, this, &AA_WSCharacter::CancelClimb);

        // 캐릭터 변경
        EnhancedInputComponent->BindAction(CharacterSwapOneAction, ETriggerEvent::Started, this, &AA_WSCharacter::SwapCharacterOne);
        EnhancedInputComponent->BindAction(CharacterSwapTwoAction, ETriggerEvent::Started, this, &AA_WSCharacter::SwapCharacterTwo);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
    }
}

void AA_WSCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector MovementVector = Value.Get<FVector>();


   
       if (Controller != nullptr)
        {
            // find out which way is forward
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);

            // get forward vector
            const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

            // get right vector 
            const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

            // add movement 
            FVector Direction;
            if (CilmMovementComponent->IsClimbing())
            {                        // 외적
                Direction = FVector::CrossProduct(CilmMovementComponent->GetClimbSurfaceNormal(), -GetActorRightVector());
                AddMovementInput(Direction, MovementVector.Y);
            }
            else
            {
                AddMovementInput(ForwardDirection, MovementVector.Y);
            }
            FVector Direction1;
            if (CilmMovementComponent->IsClimbing())
            {
                Direction1 = FVector::CrossProduct(CilmMovementComponent->GetClimbSurfaceNormal(), GetActorUpVector());
                AddMovementInput(Direction1, MovementVector.X);
            }
            else
            {
                AddMovementInput(RightDirection, MovementVector.X);
            }
           
            
        }
    

    
}





void AA_WSCharacter::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AA_WSCharacter::SwapCharacterOne()
{
    if (bIsClimbing || bIsGliding || GetCharacterMovement()->IsFalling())
    {
        return;
    }
    
    if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
    {
        CharacterSubsystem->SpawnAsCharacter(0);
    }
}

void AA_WSCharacter::SwapCharacterTwo()
{
    if (bIsClimbing || bIsGliding || GetCharacterMovement()->IsFalling())
    {
        return;
    }
    
    if (UCharacterSubsystem* CharacterSubsystem = GetGameInstance()->GetSubsystem<UCharacterSubsystem>())
    {
        CharacterSubsystem->SpawnAsCharacter(1);
    }
}

void AA_WSCharacter::OnCrouch()
{
    bIsCrouchPressing = true;

    if (!CanBeCrouched())
    { 
        return;
    }

    Crouch();
}

void AA_WSCharacter::UnOnCrouch()
{
    bIsCrouchPressing = false;

    // 지금 당장 숙이고 있는가?
    if (!GetMovementComponent()->IsCrouching()) 
    {
        return;
    }

    UnCrouch();

    // 상충되는 빠르게 달리기 키가 눌려있다면 빠르게 달리기로 상태 변환
    if (bIsFastRunningPressing) 
    {
        FastRun();
    }
}

void AA_WSCharacter::FastRun()
{
    bIsFastRunningPressing = true;

    if (!CanFastRun()) 
    {
        return;
    }

    bIsFastRunning = true;
	OnFastRun.Broadcast();
    GetCharacterMovement()->MaxWalkSpeed = GetStatsComponent()->GetMovementStats().GetFastMaxSpeed();
}

void AA_WSCharacter::UnFastRun()
{
    bIsFastRunningPressing = false;

    if (!bIsFastRunning) 
    {
        return;
    }

    bIsFastRunning = false;
	OffFastRun.Broadcast();
    GetCharacterMovement()->MaxWalkSpeed = GetStatsComponent()->GetMovementStats().GetNormalMaxSpeed();

    if (bIsWalkingPressing) 
    {
        // 상충되는 걷기 키가 눌려있다면 걷기로 상태 변환
        GoOnWalk();
    }
    else if (bIsCrouchPressing) 
    {
        // 상충되는 숙이기 키가 눌려 있다면 숙이기로 상태 변환
        OnCrouch();
    }
}

void AA_WSCharacter::GoOnWalk()
{
    bIsWalkingPressing = true;

    if (!CanWalk())
    {
        return;
    }

    OnWalk.Broadcast();
    bIsWalking = true;
    GetCharacterMovement()->MaxWalkSpeed = GetStatsComponent()->GetMovementStats().GetWalkSpeed();
}

void AA_WSCharacter::GoOffWalk() 
{
    bIsWalkingPressing = false;
 
    if (!bIsWalking) 
    {
        return;
    }

    OffWalk.Broadcast();
    bIsWalking = false;
    GetCharacterMovement()->MaxWalkSpeed = GetStatsComponent()->GetMovementStats().GetNormalMaxSpeed();

    // 상충되는 빠르게 달리기가 눌려있다면 빠르게 달리기로 상태변환
    if (bIsFastRunningPressing) 
    {
        FastRun();
    }
}

void AA_WSCharacter::OnJump()
{
    UCharacterMovementComponent* CharacterComponent = GetCharacterMovement();
    
    if (CilmMovementComponent->IsClimbing())
    {
        CilmMovementComponent->TryClimbDashing();
    }
    else if (!bIsGliding && !CharacterComponent->IsFalling())
    {
        Jump();
    }
    
    else if (CharacterComponent->IsFalling())
    {
        if (CanGlide())
        {
            bIsGliding = !bIsGliding;
            
        }
        else if (bIsGliding)
        {
            bIsGliding = !bIsGliding;
            
        }
    }
}

void AA_WSCharacter::StopOnJump()
{
    StopJumping();
}

void AA_WSCharacter::ZoomWheel(const FInputActionValue& Value)
{
    USpringArmComponent* SpringArm = GetCameraBoom();
    //if (!SpringArm) { ensure(false); return; }
    const float ActionValue = Value.Get<float>();
    if (FMath::IsNearlyZero(ActionValue)) { return; }
    SpringArm->TargetArmLength = FMath::Clamp(SpringArm->TargetArmLength + (ActionValue*50), 50.f, 500.f);
}



bool AA_WSCharacter::CanGlide()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    FHitResult HitResult;
    FCollisionQueryParams Params;
    UCharacterMovementComponent* CharacterComponent = GetCharacterMovement();
    const FNavAgentProperties& AgentProps = CharacterComponent->GetCharacterOwner()->GetNavAgentPropertiesRef();
    const float SearchRadius = AgentProps.AgentRadius * 2.0f;
    const FVector Start = GetActorLocation();
    const FVector End = Start - FVector(0, 0, 300);
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

    //DrawDebugLine
    {
        FColor LineColor = FColor::Red;
        if (!bHit)
        {
            LineColor = FColor::Green;
        }
        DrawDebugLine(GetWorld(), Start, End, LineColor, false, 3.f, 0U, 2.0f);
    }
    return !bHit;
}



void AA_WSCharacter::FindAndTake()
{
    TArray<AActor*> OverlapResults = GetWorld()->GetSubsystem<UWorldStatusSubsystem>()->GetNearestItems();

    if (!OverlapResults.IsEmpty())
    {
        for (AActor* OverlapActor : OverlapResults)
        {
            const UC_WSPickUp* PickUpComponent = OverlapActor->GetComponentByClass<UC_WSPickUp>();
            // Item trace channel에 pick up component가 없는 물체가 발견된 경우
            ensureAlwaysMsgf(PickUpComponent, TEXT("Item does not have the pick up component!"));

            if (PickUpComponent)
            {
                // 이미 소유자가 있는 물건이므로 패스
                if (PickUpComponent->IsTaken())
                {
                    continue;
                }

                PickUpComponent->OnPickUp.Broadcast(this);
                break;
            }
        }
    }
}

void AA_WSCharacter::CheckItemAndDrop()
{
    // 인벤토리가 비어있지 않다면
    if (!Inventory->GetItems().IsEmpty())
    {
        // 인벤토리의 첫번째 아이템을 버린다
        for (const auto& [Metadata, _] : Inventory->GetItems())
        {
            // 무기는 버릴 수 없음
            if (Metadata->IsItem<EItemType::Weapon>())
            {             
                continue;
            }

            AA_WSItem* NewItem = GetWorld()->SpawnActorDeferred<AA_WSItem>
                (
                    AA_WSItem::StaticClass(),
                    FTransform::Identity,
                    nullptr,
                    nullptr,
                    ESpawnActorCollisionHandlingMethod::AlwaysSpawn
                );

            FVector Origin, Extents, Forward;
            GetActorBounds(true, Origin, Extents);
            Forward = GetActorForwardVector();
            
            const FRotator ToForwardRotator = Forward.Rotation();
            const FVector XYGap{ Extents.X + 1.f, Extents.Y + 1.f, 0.f };
            const FVector RotatedExtent = ToForwardRotator.RotateVector(XYGap);

            FTransform ItemTransform{ FQuat::Identity, GetActorLocation() + RotatedExtent, FVector::OneVector };

            NewItem->SetAssetName(Metadata->GetAssetName());
            NewItem->GetComponentByClass<UProjectileMovementComponent>()->SetVelocityInLocalSpace(Forward);
            NewItem->GetComponentByClass<UProjectileMovementComponent>()->InitialSpeed = 150.f;
            NewItem->GetComponentByClass<UProjectileMovementComponent>()->bSimulationEnabled = true;
            NewItem->FinishSpawning(ItemTransform, false);
            Inventory->RemoveItem(NewItem, 1);
            break;
        }
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Log, TEXT("Character does not have Item, discard"));
    }
}

void AA_WSCharacter::Climb()
{
    
    CilmMovementComponent->TryClimbing();
}

void AA_WSCharacter::CancelClimb()
{
    CilmMovementComponent->CancelClimbing();
}




