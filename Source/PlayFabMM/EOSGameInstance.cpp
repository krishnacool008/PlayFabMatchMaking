// Fill out your copyright notice in the Description page of Project Settings.


#include "EOSGameInstance.h"

#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/GameplayStatics.h"

UEOSGameInstance::UEOSGameInstance()
{
	
}

void UEOSGameInstance::Init()
{
	Super::Init();
	OnlineSubSystem = IOnlineSubsystem::Get();

	//UE_LOG(LogTemp, Warning, TEXT("Init Caalled"));
	//Login();
	
}

void UEOSGameInstance::Login()
{
	if (OnlineSubSystem)
	{
		IOnlineIdentityPtr Identity = OnlineSubSystem->GetIdentityInterface();
		if (Identity)
		{
			FOnlineAccountCredentials UserCredential;
			
			/*UserCredential.Id = FString("127.0.0.1:8081");
			UserCredential.Token = FString("TestName");
			UserCredential.Type = FString("developer");*/

			UserCredential.Id = FString();
			UserCredential.Token = FString();
			UserCredential.Type = FString("accountportal");

			Identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSGameInstance::OnLoginComplete);
			Identity->Login(0, UserCredential);
		}
	}
}

void UEOSGameInstance::CreateSession()
{
	if (OnlineSubSystem && bIsLoggedIn)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
		{
			UE_LOG(LogTemp, Warning, TEXT("Creating Session As Login Status is -> ((%d))"), bIsLoggedIn);
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsDedicated = false;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bUsesPresence = true;
			SessionSettings.bIsLANMatch = false;
			SessionSettings.NumPublicConnections = 5;
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bAllowJoinViaPresence = true;
			SessionSettings.bUseLobbiesIfAvailable = true;
			SessionSettings.Set(SEARCH_KEYWORDS, FString("XYZLobby"), EOnlineDataAdvertisementType::ViaOnlineService);

			SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnCreateSessionComplete);
			SessionPtr->CreateSession(0, FName("Test Session"), SessionSettings);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can not create session not logged in ((%d))"), bIsLoggedIn);
	}
}

void UEOSGameInstance::DestroySession()
{
	if (OnlineSubSystem && bIsLoggedIn)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
		{
			SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::DestroySessionComplete);
			SessionPtr->DestroySession(FName("Test Session"));
		}
	}
}

void UEOSGameInstance::FindSession()
{
	if (OnlineSubSystem && bIsLoggedIn)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
		{
			SearchSettings = MakeShareable(new FOnlineSessionSearch());
			SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, FString("XYZLobby"), EOnlineComparisonOp::Equals);
			SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
			
			SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnFindSessionsComplete);
			SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
		}
	}
}


void UEOSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccesful)
{
	UE_LOG(LogTemp, Warning, TEXT("Was Successfull -> %d"), bWasSuccesful);
	UEngine* Engine = GetEngine();
	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString().Printf(TEXT("Was Successfull -> %d"), bWasSuccesful));
}

void UEOSGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("Logged in Status -> %d"), bWasSuccessful);
	UEngine* Engine = GetEngine();
	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString().Printf(TEXT("Logged in Status -> %d"), bWasSuccessful));
	bIsLoggedIn = bWasSuccessful;
}

void UEOSGameInstance::DestroySessionComplete(FName SessionName, bool Successfull)
{
	UEngine* Engine = GetEngine();
	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString().Printf(TEXT("Session Destroy Status -> %d"), Successfull));
}

void UEOSGameInstance::OnFindSessionsComplete(bool Successfull)
{
	if (Successfull)
	{
		UEngine* Engine = GetEngine();
		Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString().Printf(TEXT("Total Sessions Found -> %d"), SearchSettings->SearchResults.Num()));
		UE_LOG(LogTemp, Warning, TEXT("Total Sessions Found -> %d"), SearchSettings->SearchResults.Num());

		if (OnlineSubSystem && bIsLoggedIn)
		{
			if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
			{
				if(SearchSettings->SearchResults.Num())
				{
					SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSGameInstance::OnJoinSessionComplete);
					SessionPtr->JoinSession(0, FName("Test Session"), SearchSettings->SearchResults[0]);
				}
			}
		}
			
		
	}
}

void UEOSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Session Joined"));
	UEngine* Engine = GetEngine();
	Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString().Printf(TEXT("Session Joined")));
	
	if (OnlineSubSystem && bIsLoggedIn)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubSystem->GetSessionInterface())
		{
			FString ConnectionInfo = FString();
			SessionPtr->GetResolvedConnectString(SessionName, ConnectionInfo);
			if (!ConnectionInfo.IsEmpty())
			{
				if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					PC->ClientTravel(ConnectionInfo, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
}
