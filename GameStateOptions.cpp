#include "GameStateOptions.h"

#include "ASCII.h"
#include "Game.h"

#include <iomanip>

GameStateOptions::GameStateOptions (Game * game)
: GameState(game)
{
}

GameState::StateAction GameStateOptions::processInput ()
{
    int inputNumber = mGame->prompt().promptNumber(
        "Enter choice: ");

    switch (inputNumber)
    {
    case 1:
        return GameState::Pop {};

    case 2:
        int inputChoice = mGame->prompt().promptChoice(
            "Enter new color mode: ",
            ColorModeToVector());
        mGame->options().colorMode() =
            static_cast<ColorMode>(inputChoice);
        ASCIIEscape::setEnabled(inputChoice != 0);
        break;
    }

    return GameState::Keep {};
}

void GameStateOptions::processUpdate ()
{ }

void GameStateOptions::processEvents ()
{
    for (auto const & event: mGame->events())
    {
        std::visit(*this, event);
    }
}

void GameStateOptions::draw ()
{
    mGame->display()->clear();

    mGame->output() << "Options" << std::endl;
    mGame->output() << std::endl;

    mGame->output() << std::setw(15) << "Colors: "
        << ColorModeToString(mGame->options().colorMode())
        << std::endl;

    mGame->output() << std::endl;
    mGame->output() << "1. Done." << std::endl;
    mGame->output() << "2. Change colors." << std::endl;
    mGame->output() << std::endl;
}
