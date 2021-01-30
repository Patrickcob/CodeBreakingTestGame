// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();
    

    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordLists/HiddenWordList.txt");
    FFileHelper::LoadFileToStringArray(Words, *WordListPath);
    Words = GetValidWords(Words); //Preload les isograms
    
    SetupGame(); //Setup initial

}

void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{
    if (bGameOver)
    {
        ClearScreen();
        SetupGame();
    }
    else
    {
        ProcessGuess(PlayerInput); //Check input
    }
}

void UBullCowCartridge::SetupGame()
{
    //Message de bienvenue
    HiddenWord = Words[FMath::RandRange(0,Words.Num() - 1)];
    Lives = HiddenWord.Len();
    bGameOver = false;

    PrintLine(TEXT("Bonjour! Bienvenue au jeu d'hétérogramme."));
    PrintLine(TEXT("Decouvrez le mot à %i lettres!"), HiddenWord.Len());
    PrintLine(TEXT("Vous avez %i vies..."), Lives);
    PrintLine(TEXT("Écrivez votre mot\nAppuyez sur entrée pour continuer..."));
    //PrintLine(TEXT("Le mot caché est : %s"), *HiddenWord); //debug

}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("\nAppuyez sur entrée pour jouer à nouveau..."));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    if (Guess == HiddenWord)
    {
        PrintLine(TEXT("Vous avez raison!"));
        EndGame();
        return;
    }

    if(!IsIsogram(Guess))
    {
        PrintLine(TEXT("Le mot ne doit pas \navoir de lettre en double"));
        return;
    }

    if (HiddenWord.Len() != Guess.Len())
    {
        PrintLine(TEXT("Votre mot comporte %i lettre \net le mot caché : %i"), Guess.Len(), HiddenWord.Len());
        return;
    }

    PrintLine(TEXT("Vous avez perdu 1 vie, il vous reste %i vie(s)"), --Lives);
    PrintLine(TEXT("Réessayez!"));

    if(Lives <= 0)
    {
        ClearScreen();
        PrintLine(TEXT("Vous n'avez plus de vies, vous avez perdu!"));
        PrintLine(TEXT("Le mot caché était %s."), *HiddenWord);    
        EndGame();
        return;
    }

    FBullCowCount Score = GetBullCows(Guess);

    PrintLine(TEXT("Vous avez %i Boeufs et %i Vaches"), Score.Bulls, Score.Cows);
}

bool UBullCowCartridge::IsIsogram(const FString& Mot) const
{
    for (int32 i = 0; i < Mot.Len(); i++)
    {
        for (int32 j = i + 1; j < Mot.Len(); j++)
        {
            if(Mot[i] == Mot[j])
            {
                return false;
            }
        }
    }
    return true;
    
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WorldList) const
{
    TArray<FString> ValidWords; //seulement les Isogram


    for (FString mot : WorldList)
    {
        if(mot.Len() >= 4 && mot.Len() <= 8 && IsIsogram(mot))
        {
            ValidWords.Emplace(mot);
        }
    }
    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const 
{
    FBullCowCount Count;

    for (int32 i = 0; i < Guess.Len(); i++)
    {
        if(Guess[i] == HiddenWord[i])
        {
            Count.Bulls++;
            continue;
        }

        for (int32 j = 0; j < Guess.Len(); j++)
        {
            if (Guess[i] == HiddenWord[j])
            {
                Count.Cows++;
                break;
            }
        }
    }
    return Count;
}