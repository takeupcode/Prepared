#ifndef COMMANDSELECTCHARACTER_H
#define COMMANDSELECTCHARACTER_H

#include "Command.h"

#include <memory>
#include <string>

class CommandSelectCharacter : public Command
{
public:
    static std::unique_ptr<Command> tryCreate (
        std::string const & inputString);

    GameState::StateAction execute (Game * game) const override;

private:
    CommandSelectCharacter (int characterId);

    int mCharacterId;
};

#endif // COMMANDSELECTCHARACTER_H
