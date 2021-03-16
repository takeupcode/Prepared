#ifndef COMMANDINVENTORY_H
#define COMMANDINVENTORY_H

#include "Command.h"

#include <memory>
#include <optional>
#include <string>

class CommandInventory : public Command
{
public:
    static std::unique_ptr<Command> tryCreate (
        std::string const & inputString);

    GameState::StateAction execute (Game * game) const override;

private:
    CommandInventory (
        std::optional<int> characterId = std::nullopt);

    std::optional<int> mCharacterId;
};

#endif // COMMANDINVENTORY_H
