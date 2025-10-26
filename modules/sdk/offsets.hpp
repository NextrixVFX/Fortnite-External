#pragma once

#include "DSAPI/DSAPI.h"
#include <iostream>

class Offsets
{
public:
    inline static uintptr_t UWorld = 0;
    inline static uintptr_t AcknowledgedPawn = 0;
    inline static uintptr_t Mesh = 0;
    inline static uintptr_t RootComponent = 0;
    inline static uintptr_t LocalPlayers = 0;
    inline static uintptr_t PlayerController = 0;
    inline static uintptr_t OwningGameInstance = 0;
    inline static uintptr_t PawnPrivate = 0;
    inline static uintptr_t GameState = 0;
    inline static uintptr_t PlayerId = 0;
    inline static uintptr_t PlayerState = 0;
    inline static uintptr_t PlayerArray = 0;
    inline static uintptr_t CanBeMarkedAsTeammate = 0;
    inline static uintptr_t isDBNO = 0;
    inline static uintptr_t isDying = 0;
    inline static uintptr_t isABot = 0;
    inline static uintptr_t CurrentWeapon = 0;


    inline static bool GrabOffsets()
	{
        // first we create a DSAPI object with the hash of the game. You can get the hash out of the url
        // ...dumpspace/main/Games/index.html?hash=6b77eceb <----
        DSAPI api = DSAPI("6b77eceb");

        api.downloadContent();

        UWorld = 0x17D2F6C8;// api.getOffset("OFFSET_UWORLD"); // uworld is a pointer to gworld?
        
        if (!UWorld)
            return false;

        AcknowledgedPawn = 0x350;// api.getOffset("APlayerController", "AcknowledgedPawn").offset;
        PlayerId = 0x294; // api.getOffset("APlayerState", "PlayerId").offset
        PlayerState = 0x2b0; // api.getOffset("APawn", "PlayerState").offset;
        Mesh = 0x328;// api.getOffset("ACharacter", "Mesh").offset;
        RootComponent = 0x1b0;// api.getOffset("AActor", "RootComponent").offset;
        PlayerController = 0x30;// api.getOffset("UPlayer", "PlayerController").offset;
        LocalPlayers = 0x38;//  api.getOffset("UGameInstance", "LocalPlayers").offset;
        OwningGameInstance = 0x248;//  api.getOffset("UWorld", "OwningGameInstance").offset;
        PawnPrivate = 0x320;// api.getOffset("APlayerState", "PawnPrivate").offset;
        GameState = 0x1D0;// api.getOffset("UWorld", "GameState").offset;
        PlayerArray = 0x2C0;// api.getOffset("AGameStateBase", "PlayerArray").offset;
        isDBNO = 0x841; // api.getOffset("AFortPawn", "bIsDBNO").offset;
        isABot = 0x2b2;
        isDying = 0x728; // api.getOffset("AFortPawn", "bIsDying").offset;
        CurrentWeapon = 0x990; // api.getOffset("AFortPawn", "AFortWeapon").offset;
        return true;
	}
};