#include "GameStateEnding.h"

#include "Game.h"

GameStateEnding::GameStateEnding (Game * game)
: GameState(game), mGameOver(false)
{ }

GameState::StateAction GameStateEnding::processInput ()
{
    bool inputBool = mGame->prompt().promptYesOrNo(
        "Are you sure you want to quit?");
    if (inputBool)
    {
        mGame->quit();
        mGameOver = true;
        return GameState::Keep {};
    }

    return GameState::Pop {};
}

void GameStateEnding::processUpdate ()
{ }

void GameStateEnding::processEvents ()
{
    for (auto const & event: mGame->events())
    {
        std::visit(*this, event);
    }
}

void GameStateEnding::draw ()
{
    mGame->display()->clear();

    mGame->output() << (mGameOver ?
        "Game over" :
        "Quitting Prepared") << std::endl;
    mGame->output() << std::endl;
    mGame->output() << std::endl;
}
