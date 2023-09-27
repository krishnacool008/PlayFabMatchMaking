// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EOSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PLAYFABMM_API UEOSGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UEOSGameInstance();
	virtual void Init() override;

protected:
	class IOnlineSubsystem* OnlineSubSystem;
	
	

public:
	UFUNCTION(BlueprintCallable)
	void CreateSession();
	UFUNCTION(BlueprintCallable)
	void DestroySession();
	UFUNCTION(BlueprintCallable)
	void Login();
	UFUNCTION(BlueprintCallable)
	void FindSession();
	
	TSharedPtr<class FOnlineSessionSearch> SearchSettings;
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccesful);
	bool bIsLoggedIn = false;

private:
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	void DestroySessionComplete(FName SessionName, bool Successfull);
	void OnFindSessionsComplete(bool Successfull);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
};
