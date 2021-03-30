#include "Game.h"
#include "Prompt.h"

#include <iostream>

int main ()
{
    bool continuePlaying = true;
    while (continuePlaying)
    {
        Game game(std::cout, std::cin);

        game.play();

        continuePlaying = game.prompt().promptYesOrNo(
            "Would you like to play again?");
    }

    return 0;
}
