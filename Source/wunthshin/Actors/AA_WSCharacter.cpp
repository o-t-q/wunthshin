
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
#include "wunthshin/AnimInstance/BaseAnimInstance.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "Engine/OverlapResult.h"
#include "Item/A_WSItem.h"
#include "Item/Weapon/A_WSWeapon.h"
#include "wunthshin/Components/CharacterStats/CharacterStatsComponent.h" 
#include "wunthshin/Data/CharacterTableRow.h"
#include "InputMappingContext.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "wunthshin/Data/ItemMetadata/SG_WSItemMetadata.h"

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

AA_WSCharacter::AA_WSCharacter()
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
    Inventory = CreateDefaultSubobject<UC_WSInventory>(TEXT("Inventory"));
    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

    // Create CharacterStats
    CharacterStatsComponent = CreateDefaultSubobject<UCharacterStatsComponent>(TEXT("CharacterStatsComponent"));

    RightHandWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightHandWeapon"));
	
	CameraBoom->bEnableCameraLag = true;

}

void AA_WSCharacter::HandleStaminaDepleted()
{
    // protected UnFastRun을 CharacterStatsComponent에서 호출하기 위함.
    UnFastRun();
}

void AA_WSCharacter::K2_FastRun()
{
    FastRun();
}

void AA_WSCharacter::K2_UnFastRun()
{
    UnFastRun();
}

UScriptStruct* AA_WSCharacter::GetTableType() const
{
    return FCharacterTableRow::StaticStruct();
}

void AA_WSCharacter::ApplyAsset(const FDataTableRowHandle& InRowHandle)
{
    if (InRowHandle.IsNull()) return;

    const FCharacterTableRow* Data = InRowHandle.GetRow<FCharacterTableRow>(TEXT(""));

    if (!Data)
    {
        return;
    }

    // todo: 캐릭터의 이름, 아이콘

    if (Data->SkeletalMesh)
    {
        GetMesh()->SetSkeletalMesh(Data->SkeletalMesh);
    }

    if (Data->AnimInstance)
    {
        GetMesh()->SetAnimInstanceClass(Data->AnimInstance);
    }
}

void AA_WSCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();

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
    FetchAsset(this, AssetName);
}

void AA_WSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UCharacterMovementComponent* CharacterComponent = GetCharacterMovement();
    if (bCanGlide)
    {
        //GetCharacterMovement()->GravityScale = 0.05f;
        GetCharacterMovement()->Velocity.Z = FMath::Clamp(GetCharacterMovement()->Velocity.Z, -100.f, 0.0f);
    }
    

    if (!CharacterComponent->IsFalling())
    {
        bCanGlide = false;
        StopJumping();
    }
    
}

bool AA_WSCharacter::Take(UC_WSPickUp* InTakenComponent)
{
    // 아이템을 저장
    AA_WSItem* Item = Cast<AA_WSItem>(InTakenComponent->GetOwner());
    ensure(Item);
    UE_LOG(LogTemplateCharacter, Log, TEXT("Pick up item: %s"), *Item->GetName());

    // 손이 비어있고, 무기를 잡으려 할때
    if (const AA_WSWeapon* WeaponCast = Cast<AA_WSWeapon>(Item);
        WeaponCast && !RightHandWeapon->GetChildActor())
    {
        // todo: 동적으로 변경될떄의 대응
        // 테스트용으로 블루프린트 클래스를 사용하기 때문에 빈 클래스를 쓰게 될 경우
        // 대상의 아이템 및 무기의 이름에 따라 에셋을 다시 설정해줘야 함
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
        RightHandWeapon->GetChildActor()->GetComponentByClass<UC_WSPickUp>()->SetActive(false, false);

        // 충돌 반응 비활성화, overlap으로 반응하는 아이템 프로필로 설정
        RightHandWeapon->GetChildActor()->GetComponentByClass<UMeshComponent>()->SetCollisionProfileName("ItemEquipped");
    }
    
    // 인벤토리로 무기 또는 아이템 저장
    Inventory->AddItem(Item);
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
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
    }
}

void AA_WSCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

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
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
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

void AA_WSCharacter::OnCrouch()
{
    bIsCrouchPressing = true;

    if (GetMovementComponent()->IsFalling() ||
        GetCharacterMovement()->bWantsToCrouch ||
        bIsFastRunning)
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

    // 이전 틱 + 현재 틱을 포함하여 Crouch중인 상황 (Crouch 업데이트가 lazy하게 발생함)
    if (GetCharacterMovement()->bWantsToCrouch ||
        bIsFastRunning ||
        bIsWalking) 
    {
        return;
    }

    bIsFastRunning = true;
	OnFastRun.Broadcast();
    GetCharacterMovement()->MaxWalkSpeed = 1000;
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
    GetCharacterMovement()->MaxWalkSpeed = 500;

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

    if (bIsWalking ||
        bIsFastRunning)
    {
        return;
    }

    OnWalk.Broadcast();
    bIsWalking = true;
    GetCharacterMovement()->MaxWalkSpeed = 200;
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
    GetCharacterMovement()->MaxWalkSpeed = 500;

    // 상충되는 빠르게 달리기가 눌려있다면 빠르게 달리기로 상태변환
    if (bIsFastRunningPressing) 
    {
        FastRun();
    }
}

void AA_WSCharacter::OnJump()
{
    UCharacterMovementComponent* CharacterComponent = GetCharacterMovement();
    if (!bCanGlide && !CharacterComponent->IsFalling())
    {
        Jump();
    }
    else if (CharacterComponent->IsFalling())
    {
        if (CanGlide())
        {
            bCanGlide = !bCanGlide;
            
        }
        else if (bCanGlide)
        {
            bCanGlide = !bCanGlide;
            
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
    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams QueryParams(NAME_None, false, this);
    QueryParams.AddIgnoredActors(reinterpret_cast<const TArray<AActor*>&>(Inventory->GetItems()));

    // 손에 있는 무기는 줍는 대상에서 제외 
    if (const AActor* ChildWeaponActor = RightHandWeapon->GetChildActor())
    {
        QueryParams.AddIgnoredActor(ChildWeaponActor);
    }
    
    // 반환 값은 blocking일때 참을 반환하나, overlap으로 trace channel을 쓰기 때문에 무시함
    GetWorld()->OverlapMultiByChannel
    (
        OverlapResults,
        GetActorLocation(),
        FQuat::Identity,
        ECC_GameTraceChannel2,
        FCollisionShape::MakeSphere(100.f)
    );

    if (!OverlapResults.IsEmpty())
    {
        // Overlap된 물체들을 거리 순서대로 정렬한다
        OverlapResults.Sort
        ([this](const FOverlapResult& Left, const FOverlapResult& Right)
            {
                return FVector::Distance(Left.GetActor()->GetActorLocation(), GetActorLocation()) >
                    FVector::Distance(Right.GetActor()->GetActorLocation(), GetActorLocation());
            });

        for (FOverlapResult OverlapResult : OverlapResults)
        {
            const UC_WSPickUp* PickUpComponent = OverlapResult.GetActor()->GetComponentByClass<UC_WSPickUp>();
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
