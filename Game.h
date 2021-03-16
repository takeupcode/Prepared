#ifndef GAME_H
#define GAME_H

#include "Character.h"
#include "Display.h"
#include "GameEvent.h"
#include "GameState.h"
#include "Level.h"
#include "Prompt.h"

#include <istream>
#include <memory>
#include <optional>
#include <ostream>
#include <random>
#include <stack>
#include <vector>

class Behavior;

class Game
{
public:
    Game (
        std::ostream & output,
        std::istream & input,
        unsigned int seed = 0);

    void play ();

    void quit ();

    std::ostream & output ();

    std::istream & input ();

    Display * display ();

    Level * level ();

    Prompt & prompt ();

    unsigned int seed ();

    int randomPercent ();

    std::vector<Character> & characters ();
    std::vector<Character> const & characters () const;

    std::optional<int> defaultCharacterId () const;
    void setDefaultCharacterId (int id);

    Character * findCharacter (int id);
    Character const * findCharacter (int id) const;

    std::vector<Character> & creatures ();
    std::vector<Character> const & creatures () const;

    Character * findCreature (int id);
    Character const * findCreature (int id) const;

    void addEvent (GameEvent const & event);

    std::vector<GameEvent> const & events () const;

    void spawnCharacters (
        std::vector<Character> const & characters);

    void spawnCreatures ();

    void operator () (GameState::Unknown & action);

    void operator () (GameState::Keep & action);

    void operator () (GameState::Swap & action);

    void operator () (GameState::Push & action);

    void operator () (GameState::Pop & action);

    Behavior * playerCharacterBehavior () const;

    Behavior * creatureBehavior () const;

private:
    GameState::StateAction processInput ();

    void processUpdate ();

    void processEvents ();

    void draw ();

    void placeCharacters ();

    std::ostream & mOutput;
    std::istream & mInput;
    bool mGameOver;
    Prompt mPrompt;
    std::optional<int> mDefaultCharacterId;
    std::vector<Character> mCharacters;
    std::vector<Character> mCreatures;
    std::vector<GameEvent> mEvents;
    std::stack<std::unique_ptr<GameState>> mStates;
    std::unique_ptr<Display> mDisplay;
    std::unique_ptr<Level> mLevel;
    unsigned int mSeed;
    std::mt19937 mRNG;
    std::uniform_int_distribution<std::mt19937::result_type>
        mPercent;
};

#endif // GAME_H
