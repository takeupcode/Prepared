#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <memory>
#include <variant>

class Game;

class GameState
{
public:
    struct Unknown
    { };

    struct Keep
    { };

    struct Swap
    {
        std::unique_ptr<GameState> newState;
    };

    struct Push
    {
        std::unique_ptr<GameState> newState;
    };

    struct Pop
    { };

    using StateAction = std::variant<
        Unknown,
        Keep,
        Swap,
        Push,
        Pop
    >;

    virtual ~GameState () = default;

    virtual StateAction processInput () = 0;

    virtual void processUpdate () = 0;

    virtual void processEvents () = 0;

    virtual void draw () = 0;

protected:
    GameState (Game * game)
    : mGame(game)
    { }

    Game * mGame;
};

#endif // GAMESTATE_H
