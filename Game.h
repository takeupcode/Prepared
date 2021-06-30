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
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <random>
#include <set>
#include <stack>
#include <unordered_map>
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
    void setDefaultCharacterId (int instanceId);

    GameItem * createItem (int id);

    void eraseItem (int instanceId);

    GameItem * findItem (int instanceId) const;

    std::vector<GameItem *> findItems (
        std::string const & tag) const;

    template <typename Pred>
    std::vector<GameItem *> findItemsIf (
        std::string const & tag,
        Pred pred) const;

    void addEvent (GameEvent const & event);

    std::vector<GameEvent> const & events () const;

    void spawnCharacters ();

    void spawnCreatures ();

    void addLayer (int layerId);
    void removeLayer (int layerId);
    std::vector<int> layers () const;

    void operator () (GameState::Unknown & action);

    void operator () (GameState::Keep & action);

    void operator () (GameState::Swap & action);

    void operator () (GameState::Push & action);

    void operator () (GameState::Pop & action);

    void operator () (TagAdded const & event);

    void operator () (TagRemoved const & event);

    template <typename EventVariant>
    void operator () (EventVariant const &)
    { }

private:
    GameState::StateAction processInput ();

    void processUpdate ();

    void processEvents ();

    void draw ();

    void registerComponents ();
    void registerItems ();

    void setLayerCollisionsInLevel ();

    void reset (unsigned int seed = 0);

    void createGameItemIndex (std::string const & name);

    std::ostream & mOutput;
    std::istream & mInput;
    bool mGameOver;
    Prompt mPrompt;
    std::optional<int> mDefaultCharacterId;
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

    using GameItemMap =
        std::unordered_map<int, std::unique_ptr<GameItem>>;
    using GameItemIndex = std::set<int>;
    using GameItemIndices = std::set<std::string>;
    using GameItemIndicesMap = std::map<std::string, GameItemIndex>;
    using GameItemShortcutMap = std::map<int, int>;
    GameItemMap mGameItems;
    GameItemIndices mGameItemIndices;
    GameItemIndicesMap mGameItemIndicesMap;
    GameItemShortcutMap mGameItemShortcutMap;
};

template <typename Pred>
std::vector<GameItem *> Game::findItemsIf (
    std::string const & tag,
    Pred pred) const
{
    std::vector<GameItem *> result;

    if (mGameItemIndices.find(tag) != mGameItemIndices.end())
    {
        auto iter = mGameItemIndicesMap.at(tag).cbegin();
        while (iter != mGameItemIndicesMap.at(tag).cend())
        {
            auto item = findItem(*iter);
            if (item != nullptr && pred(item))
            {
                result.push_back(item);
            }
            ++iter;
        }
    }

    return result;
}

#endif // GAME_H
