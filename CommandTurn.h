#ifndef COMMANDTURN_H
#define COMMANDTURN_H

#include "Command.h"
#include "Direction.h"

#include <memory>
#include <optional>
#include <string>

class CommandTurn : public Command
{
public:
    static std::unique_ptr<Command> tryCreate (
        std::string const & inputString);

    GameState::StateAction execute (Game * game) const override;

private:
    CommandTurn (
        Direction direction,
        std::optional<int> characterId = std::nullopt);

    Direction mDirection;
    std::optional<int> mCharacterId;
};

#endif // COMMANDTURN_H
