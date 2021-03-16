#include "GameStateStarting.h"

#include "Game.h"
#include "GameStateExploring.h"

#include <iomanip>

Character createCharacter (
    Game * game,
    int id,
    char symbol)
{
    Behavior * behavior = game->playerCharacterBehavior();
    Character character(id, symbol, behavior);
    character.setMaxHealth(20, false);
    character.setCurrentHealth(20, false);
    character.setAttackDamage(4);

    return character;
}

GameStateStarting::GameStateStarting (Game * game)
: GameState(game)
{
    mCharacters.push_back(createCharacter(game, 1, '1'));
}

GameState::StateAction GameStateStarting::processInput ()
{
    int inputNumber = mGame->prompt().promptNumber(
        "Enter choice: ");

    switch (inputNumber)
    {
    case 1:
        mGame->spawnCharacters(mCharacters);
        mGame->setDefaultCharacterId(mCharacters[0].id());
        mGame->spawnCreatures();
        return GameState::Swap {std::unique_ptr<GameState>(
            new GameStateExploring(mGame))};

    case 2:
        std::string inputString = mGame->prompt().promptText(
            "Enter symbol for new character: ");
        mCharacters.push_back(createCharacter(
            mGame,
            mCharacters.size() + 1,
            inputString[0]));
        break;
    }

    return GameState::Keep {};
}

void GameStateStarting::processUpdate ()
{ }

void GameStateStarting::processEvents ()
{
    for (auto const & event: mGame->events())
    {
        std::visit(*this, event);
    }
}

void GameStateStarting::draw ()
{
    mGame->display()->clear();

    mGame->output() << "Welcome to Prepared" << std::endl;
    mGame->output() << std::endl;

    mGame->output() << std::setw(10) << "Character"
        << std::setw(10) << "Symbol" << std::endl;
    for (auto const & character: mCharacters)
    {
        mGame->output() << std::setw(10) << character.id()
            << std::setw(10) << character.symbol() << std::endl;
    }

    mGame->output() << std::endl;
    mGame->output() << "1. Play game." << std::endl;
    mGame->output() << "2. Add character." << std::endl;
    mGame->output() << std::endl;
}
