#ifndef COMMANDATTACK_H
#define COMMANDATTACK_H

#include "Command.h"

#include <memory>
#include <optional>
#include <string>

class CommandAttack : public Command
{
public:
    static std::unique_ptr<Command> tryCreate (
        std::string const & inputString);

    GameState::StateAction execute (Game * game) const override;

private:
    CommandAttack (
        std::optional<int> characterId = std::nullopt);

    std::optional<int> mCharacterId;
};

#endif // COMMANDATTACK_H
