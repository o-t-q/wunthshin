
#include "AA_WSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/StaticMeshComponent.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"
#include "wunthshin/Components/Inventory/C_WSInventory.h"
#include "Engine/OverlapResult.h"
#include "Item/A_WSItem.h"
#include "Item/Weapon/A_WSWeapon.h"
#include "wunthshin/Components/CharacterStats/CharacterStatsComponent.h" 

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AwunthshinCharacter

const FName AA_WSCharacter::RightHandWeaponSocketName = TEXT("WeaponProp02");

AA_WSCharacter::AA_WSCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

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

bool AA_WSCharacter::Take(UC_WSPickUp* InTakenComponent)
{
    // 이미 아이템을 소유하고 있는 경우
    //if (Item)
    //{
    //   // todo: 테스트 용도, 손이 바빠도 인벤토리에 넣을 수 있음
    //   return false;
    //}

    // 아이템을 저장
    AA_WSItem* Item = Cast<AA_WSItem>(InTakenComponent->GetOwner());
    ensure(Item);
    UE_LOG(LogTemplateCharacter, Log, TEXT("Pick up item: %s"), *Item->GetName());

    if (const AA_WSWeapon* WeaponCast = Cast<AA_WSWeapon>(Item);
        WeaponCast && !RightHandWeapon->GetChildActor())
    {
        // todo: 동적으로 변경될떄의 대응
        // 테스트용으로 블루프린트 클래스를 사용하기 때문에 빈 클래스를 쓰게 될 경우
        // 대상의 아이템 및 무기의 이름에 따라 에셋을 다시 설정해줘야 함
        RightHandWeapon->SetChildActorClass(WeaponCast->GetClass());

        // 손에 있는 무기를 주울 수 없도록 pick up component를 비활성화
        RightHandWeapon->GetChildActor()->GetComponentByClass<UC_WSPickUp>()->SetActive(false, false);

        // 충돌 반응 비활성화, overlap으로 반응하는 아이템 프로필로 설정
        RightHandWeapon->GetChildActor()->GetComponentByClass<UShapeComponent>()->SetCollisionProfileName("ItemEquipped");
    }
    
    Inventory->AddItem(Item);
    return true;
}

bool AA_WSCharacter::Drop(UC_WSPickUp* InTakenComponent)
{
    AA_WSItem* Item = Cast<AA_WSItem>(InTakenComponent->GetOwner());
    ensure(Item);

    if (Item)
    {
        UE_LOG(LogTemplateCharacter, Log, TEXT("Drop item: %s"), *Item->GetName());
        Inventory->RemoveItem(Item);
        return true;
    }

    return false;
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
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AA_WSCharacter::Move);

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
        const AA_WSItem* Item = Inventory->GetItems().Top();
        const UC_WSPickUp* PickUpComponent = Item->GetComponentByClass<UC_WSPickUp>();
        PickUpComponent->OnDropping.Broadcast(this);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Log, TEXT("Character does not have Item, discard"));
    }
}
