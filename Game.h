#ifndef GAME_H
#define GAME_H

#include "Display.h"
#include "GameEvent.h"
#include "GameItem.h"
#include "GameOptions.h"
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

    GameOptions & options ();

    std::optional<int> defaultCharacterId () const;
    void setDefaultCharacterId (int id);

    std::vector<GameItem> & items ();
    std::vector<GameItem> const & items () const;

    GameItem * findItem (int id);
    GameItem const * findItem (int id) const;

    void addEvent (GameEvent const & event);

    std::vector<GameEvent> const & events () const;

    void spawnCharacters (
        std::vector<GameItem> const & characters);

    void spawnCreatures ();

    void addLayer (int layerId);
    void removeLayer (int layerId);
    std::vector<int> layers () const;

    void operator () (GameState::Unknown & action);

    void operator () (GameState::Keep & action);

    void operator () (GameState::Swap & action);

    void operator () (GameState::Push & action);

    void operator () (GameState::Pop & action);

private:
    GameState::StateAction processInput ();

    void processUpdate ();

    void processEvents ();

    void draw ();

    void registerComponents ();
    void registerItems ();

    void setLayerCollisionsInLevel ();

    void placeCharacters ();

    void reset (unsigned int seed = 0);

    std::ostream & mOutput;
    std::istream & mInput;
    bool mGameOver;
    Prompt mPrompt;
    std::optional<int> mDefaultCharacterId;
    unsigned int mCharacterCount;
    std::vector<GameItem> mGameItems;
    std::vector<GameEvent> mEvents;
    std::vector<int> mLayerIds;
    std::stack<std::unique_ptr<GameState>> mStates;
    std::unique_ptr<Display> mDisplay;
    std::unique_ptr<Level> mLevel;
    unsigned int mOriginalSeed;
    unsigned int mSeed;
    std::mt19937 mRNG;
    std::uniform_int_distribution<std::mt19937::result_type>
        mPercent;
    GameOptions mOptions;
};

#endif // GAME_H
