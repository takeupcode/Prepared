#ifndef COMMANDMOVE_H
#define COMMANDMOVE_H

#include "Command.h"
#include "Direction.h"

#include <memory>
#include <optional>
#include <string>

class CommandMove : public Command
{
public:
    static std::unique_ptr<Command> tryCreate (
        std::string const & inputString);

    GameState::StateAction execute (Game * game) const override;

private:
    CommandMove (
        Direction direction,
        bool turn = false,
        std::optional<int> characterId = std::nullopt);

    Direction mDirection;
    bool mTurn;
    std::optional<int> mCharacterId;
};

#endif // COMMANDMOVE_H
