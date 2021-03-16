#include "Game.h"
#include "Prompt.h"

#include <iostream>

int main ()
{
    Prompt prompt(std::cout, std::cin);

    bool continuePlaying = true;
    while (continuePlaying)
    {
        Game game(std::cout, std::cin);

        game.play();

        continuePlaying =
            prompt.promptYesOrNo("Would you like to play again?");
    }

    return 0;
}
