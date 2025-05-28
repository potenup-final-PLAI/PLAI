// Fill out your copyright notice in the Description page of Project Settings.


#include "InputComp.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "JsonObjectConverter.h"
#include "AI/NavigationSystemBase.h"
#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PLAI/Item/GameState/GameStateOpen.h"
#include "PLAI/Item/ItemComp/InvenComp.h"
#include "PLAI/Item/Login/LoginComp.h"
#include "PLAI/Item/Npc/NpcCharacter.h"
#include "PLAI/Item/TestPlayer/TestPlayer.h"
#include "PLAI/Item/TestPlayer/TurnPlayer.h"
#include "PLAI/Item/TestPlayer/TurnComp/TurnComp.h"
#include "PLAI/Item/Turn/TurnMonsterWorld/TurnMonsterWorld.h"
#include "PLAI/Item/Turn/TurnMoster/TurnMonster.h"
#include "PLAI/Item/Turn/TurnTile/TurnTile.h"
#include "PLAI/Item/UI/Character/UIChaStat.h"
#include "PLAI/Item/UI/Inventory/EquipInven/EquipInven.h"
#include "PLAI/Item/UI/Inventory/InputUi/InputUi.h"
#include "PLAI/Item/UI/Inventory/ItemInven/ItemInven.h"
#include "PLAI/Item/UI/Turn/UiTurn.h"


// Sets default values for this component's properties
UInputComp::UInputComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInputComp::BeginPlay()
{
	Super::BeginPlay();

	TestPlayer = Cast<ATestPlayer>(GetOwner());
	Pc = Cast<APlayerController>(TestPlayer->GetController());
	TestPlayer->CameraBoom->SetWorldRotation(FRotator(-45,-90,0));

    if (!Pc) return;

	BindInputActions();
	SetMappingContext();
	
	// FTimerHandle TimerHandle;
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	// {},1.0f,false);
}



void UInputComp::BindInputActions()
{
	if (UEnhancedInputComponent* InputComp = Cast<UEnhancedInputComponent>(TestPlayer->InputComponent))
	{
		InputComp->BindAction(IE_Equip, ETriggerEvent::Started, this, &UInputComp::On_Equip);
		InputComp->BindAction(IE_Inven, ETriggerEvent::Started, this, &UInputComp::On_Inven);
		InputComp->BindAction(IE_Stat, ETriggerEvent::Started, this, &UInputComp::On_Stat);
		InputComp->BindAction(IE_LeftMouse, ETriggerEvent::Started, this, &UInputComp::On_LeftMouseStart);
		InputComp->BindAction(IE_LeftMouse, ETriggerEvent::Triggered, this, &UInputComp::On_LeftMouseTriggered);
		InputComp->BindAction(IE_LeftMouse, ETriggerEvent::Completed, this, &UInputComp::On_LeftMouseComplete);
		InputComp->BindAction(IE_MouseWheel, ETriggerEvent::Triggered, this, &UInputComp::On_MouseWheelTriggered);
		InputComp->BindAction(IE_RotateView, ETriggerEvent::Started, this, &UInputComp::On_RoatateView);
		InputComp->BindAction(IE_Map, ETriggerEvent::Started, this, &UInputComp::On_Map);
		InputComp->BindAction(IE_Jump, ETriggerEvent::Started, this, &UInputComp::Server_On_Jump);
	}
}

void UInputComp::SetMappingContext()
{
	if (!TestPlayer || !TestPlayer->IsLocallyControlled()){UE_LOG(LogTemp, Error, TEXT(
	"InputComp  서버니 클라니? [%s]"),TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라")); return;}
	else
	{ UE_LOG(LogTemp, Error, TEXT("InputComp TestPlayer is locallyControlled 있음 서버니 클라니? %s"),
			TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라")) }
	
	if (ULocalPlayer* LP = Pc->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
			{
				UE_LOG(LogTemp, Error, TEXT("InputComp TestPlayer is AddMapping [있음] 서버니 클라니? %s"),
				TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라"));
			}
		}
	}
}

void UInputComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// if (TestPlayer->IsLocallyControlled() && Pc->WasInputKeyJustPressed(EKeys::T))
	// { FString JsonString;
	// 	FJsonObjectConverter::UStructToJsonObjectString(TestPlayer->LoginComp->UserFullInfo,JsonString);
	// 	UE_LOG(LogTemp,Warning,TEXT("InputComp TKey JsonString [%s]"),*JsonString) }
}

void UInputComp::On_Equip()
{
	if (!Pc->IsLocalController()) return;
	UE_LOG(LogTemp,Warning,TEXT("InputComp 장비창 실행 [%s]"),TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라"));
	if (TestPlayer->InvenComp->MenuInven->WBP_EquipInven->GetVisibility() == ESlateVisibility::Hidden)
	{ TestPlayer->InvenComp->MenuInven->WBP_EquipInven->SetVisibility(ESlateVisibility::Visible); }
	else
	{ TestPlayer->InvenComp->MenuInven->WBP_EquipInven->SetVisibility(ESlateVisibility::Hidden); }
}

void UInputComp::On_Inven()
{
	if (!Pc->IsLocalController()) return;
	UE_LOG(LogTemp,Warning,TEXT("InputComp 인벤창 실행 [%s]"),TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라"));
	if (TestPlayer->InvenComp->MenuInven->WBP_ItemInven->GetVisibility() == ESlateVisibility::Hidden) 
	{ TestPlayer->InvenComp->MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Visible);}
	else
	{ TestPlayer->InvenComp->MenuInven->WBP_ItemInven->SetVisibility(ESlateVisibility::Hidden);}
}

void UInputComp::On_Stat()
{
	if (!Pc->IsLocalController()) return;
	UE_LOG(LogTemp,Warning,TEXT("InputComp 스텟창 실행 [%s]"),TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라"));
	if (TestPlayer->InvenComp->MenuInven->Wbp_UIChaStat->GetVisibility() == ESlateVisibility::Hidden) 
	{ TestPlayer->InvenComp->MenuInven->Wbp_UIChaStat->SetVisibility(ESlateVisibility::Visible);}
	else
	{ TestPlayer->InvenComp->MenuInven->Wbp_UIChaStat->SetVisibility(ESlateVisibility::Hidden);}
}

void UInputComp::Server_On_Jump_Implementation()
{
	On_Jump();
	TestPlayer->LaunchCharacter(FVector(0,0,1000),false,false);
}

void UInputComp::On_Jump()
{
	if (!Pc || !Pc->IsLocalController()){UE_LOG(LogTemp,Warning,TEXT("InputComp PC , PC 로칼컨트롤 아님")) return;}
	UE_LOG(LogTemp,Warning,TEXT("InputComp 점프 되는중"))
	// TestPlayer->LaunchCharacter(FVector(0,0,500),false,false);
}

void UInputComp::On_Map()
{
	UE_LOG(LogTemp,Warning,TEXT("InputComp 키누르고 OnMap 딜리게이트 실행되냐?"))
	OnInputMap.ExecuteIfBound();
}

void UInputComp::On_LeftMouseStart()
{
	if (!Pc->IsLocalController()) return;
	
	bLeftMouse = true;
	FHitResult Hit;
	Pc->GetHitResultUnderCursor(ECC_Visibility, true, Hit);

	if (Hit.GetActor()){UE_LOG(LogTemp,Warning,TEXT("InputComp 왼쪽 마우스 무슨엑터? [%s] 서클 누구?? [%s]"),
		*Hit.GetActor()->GetName(),TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라"))}

	// 턴제 전투 플레이어 선택
	if (ATurnPlayer * TurnPlayerClick = Cast<ATurnPlayer>(Hit.GetActor()))
	{
		DrawDebugSphere(GetWorld(),Hit.Location,50,10,FColor::Blue,false,1);

		if (TurnPlayerClick->bTurn == true)
		{ TurnPlayer = TurnPlayerClick; }
		return;
	}

	// 턴제 전투 플레이어 행동
	if (TurnPlayer && TurnPlayer->bTurn == true)
	{
		DrawDebugSphere(GetWorld(),Hit.Location,50,10,FColor::Red,false,1);
		
		ATurnMonster* TurnMonster = Cast<ATurnMonster>(Hit.GetActor());
		
		TurnPlayer->PlayerState(Hit.Location,TurnMonster);
		TurnPlayer = nullptr;
	}
	
	// 턴제 월드 몬스터 찾기
	if (ATurnMonsterWorld* TurnMonsterWorld = Cast<ATurnMonsterWorld>(Hit.GetActor()))
	{
		if (ATurnTile* TurnTile = Cast<ATurnTile>(UGameplayStatics::GetActorOfClass(GetWorld(),ATurnTile::StaticClass())))
		{ TestPlayer->SetActorLocation(TurnTile->GetActorLocation() + FVector(0,0,100));}
	}
	
	// Npc 찾기 창 끄기
	ANpcCharacter* NpcCharacter = Cast<ANpcCharacter>(Hit.GetActor());
	if (!NpcCharacter)
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANpcCharacter::StaticClass(), Actors);
		if (Actors.Num() == 0) return;
			
		for (AActor* Actor : Actors)
		{
			if (ANpcCharacter* Npc = Cast<ANpcCharacter>(Actor))
			{
				if (Npc->NpcUiMaster)
				{ Npc->NpcUiMaster->SetVisibility(ESlateVisibility::Hidden); }
			}
		}
	}
	
	// 턴제 플레이어
	// if (ATestPlayer* TestPlayer = Cast<ATestPlayer>(Hit.GetActor()))
	// {
	// 	
	// }
	TestPlayer->GetController()->StopMovement();
	TimeCamera = 0;
}

void UInputComp::On_LeftMouseTriggered()
{
	if (!Pc->IsLocalController()) return;

	WalkTime += GetWorld()->GetDeltaSeconds();
	FHitResult Hit;
	Pc->GetHitResultUnderCursor(ECC_Visibility, true, Hit);
	TestPlayer->AddMovementInput((Hit.Location - TestPlayer->GetActorLocation()).GetSafeNormal(),1.0f,false);


	if (bRotateView)
	{
		TimeCamera += GetWorld()->GetDeltaSeconds();
		// 오른쪽 나의 벡터와 카메라 전방 벡터 내적
		float Dot = FVector::DotProduct(FVector(TestPlayer->GetActorRightVector().X,TestPlayer->GetActorRightVector().Y,0),
			FVector(TestPlayer->CameraBoom->GetForwardVector().X,TestPlayer->CameraBoom->GetForwardVector().Y,0));
		if (Dot > 0.13)
		{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,-0.35,0)); }
		else if (Dot < -0.13)
		{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,+0.35,0)); }
		else if (Dot > 0.6)
		{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,- 7,0)); }
		else if (Dot < -0.6)
		{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,+ 7,0)); }
		float MouseX, MouseY;
		Pc->GetInputMouseDelta(MouseX, MouseY);
		FRotator CameraBoomRot = TestPlayer->CameraBoom->GetRelativeRotation();

		MousePowerX += MouseX;
		MousePowerY += MouseY;
		if (FMath::Abs(MousePowerY) > 5.f || FMath::Abs(MousePowerX) > 5.f)
		{
			CameraBoomRot.Yaw = CameraBoomRot.Yaw + 0.5 * MouseX;
			CameraBoomRot.Pitch = FMath::Clamp(CameraBoomRot.Pitch + 0.5 * MouseY, - 60.0f, 10.0f);
			TestPlayer->CameraBoom->SetRelativeRotation(CameraBoomRot); }
	}
}

void UInputComp::On_LeftMouseComplete()
{
	if (!Pc->IsLocalController()) return;

	bLeftMouse = false;
	MousePowerX = 0;
	MousePowerY = 0;
	MouseTime = 0;
}

void UInputComp::On_MouseWheelTriggered(const FInputActionValue& Value)
{
	float Axis = Value.Get<float>();
	TestPlayer->CameraBoom->TargetArmLength -= 150 * Axis;
}

void UInputComp::On_RoatateView()
{
	if (!Pc->IsLocalController()) return;
	
	UE_LOG(LogTemp, Warning, TEXT("UInputComp::On_RoatateView Bool 값 %d"),bRotateView);
	if (bRotateView == true)
	{
		TestPlayer->InvenComp->MenuInven->WBP_InputUi->InputUiHidden();
		TestPlayer->InvenComp->MenuInven->WBP_InputUi->RotateViewText->SetText(FText::FromString(TEXT("고정 시점 모드")));

		TestPlayer->CameraBoom->TargetArmLength = 2500.0f;
		// TestPlayer->CameraBoom->SetWorldRotation(FRotator(-45,TestPlayer->GetActorRotation().Yaw,0));
		TestPlayer->CameraBoom->SetWorldRotation(FRotator(-45,-90,0));
		bRotateView = false;
	}
	else
	{
		TestPlayer->InvenComp->MenuInven->WBP_InputUi->InputUiHidden();
		TestPlayer->InvenComp->MenuInven->WBP_InputUi->RotateViewText->SetText(FText::FromString(TEXT("자유 시점 모드")));
		
		bRotateView = true;
	}
}

void UInputComp::InitializeComponent()
{
	Super::InitializeComponent();

	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		Pawn->ReceivePossessed(Pc);
	}
}

// if (!TestPlayer->IsLocallyControlled()){UE_LOG(LogTemp, Error, TEXT(
// 	"InputComp  서버니 클라니? [%s]"),TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라")); return;}
// else
// { UE_LOG(LogTemp, Error, TEXT("InputComp TestPlayer is locallyControlled 있음 서버니 클라니? %s"),
// 		TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라")) }

// if (ULocalPlayer* LP = Pc->GetLocalPlayer())
// {
// 	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
// 	{
// 		Subsystem->AddMappingContext(InputMappingContext, 0);
// 		{
// 			UE_LOG(LogTemp, Error, TEXT("InputComp TestPlayer is AddMapping [있음] 서버니 클라니? %s"),
// 			TestPlayer->HasAuthority()? TEXT("서버") : TEXT("클라"));
// 		}
// 	}
// }



// if (!bRotateView) return;
//
// TimeCamera += GetWorld()->GetDeltaSeconds();
//
// // 오른쪽 나의 벡터와 카메라 전방 벡터 내적
// float Dot = FVector::DotProduct(FVector(TestPlayer->GetActorRightVector().X,TestPlayer->GetActorRightVector().Y,0),
// 	FVector(TestPlayer->CameraBoom->GetForwardVector().X,TestPlayer->CameraBoom->GetForwardVector().Y,0));
//
// if (Dot > 0.13)
// { TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,-0.35,0)); }
// else if (Dot < -0.13)
// { TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,+0.35,0)); }
//
// else if (FMath::Abs(Dot) > 0.6)
// {
// 	if (Dot > 0.6)
// 	{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,- 7,0)); }
// 	else if (Dot < -0.6)
// 	{ TestPlayer->CameraBoom->AddWorldRotation(FRotator(0,+ 7,0)); }
// }
// float MouseX, MouseY;
// Pc->GetInputMouseDelta(MouseX, MouseY);
// FRotator CameraBoomRot = TestPlayer->CameraBoom->GetRelativeRotation();
//
// MousePower += MouseY;
//
// if (FMath::Abs(MousePower) > 5.f)
// {
// 	CameraBoomRot.Pitch = FMath::Clamp(CameraBoomRot.Pitch + 0.5 * MouseY, - 60.0f, 10.0f);
// 	TestPlayer->CameraBoom->SetRelativeRotation(CameraBoomRot);
// }