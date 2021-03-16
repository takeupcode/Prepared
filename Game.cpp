#include "Game.h"

#include "BehaviorCreature.h"
#include "BehaviorPC.h"
#include "GameStateStarting.h"
#include "Location.h"

#include <chrono>
#include <ctime>

Game::Game(
    std::ostream & output,
    std::istream & input,
    unsigned int seed)
: mOutput(output), mInput(input),
  mGameOver(false),
  mPrompt(output, input),
  mSeed(seed),
  mPercent(0, 100)
{
    if (mSeed == 0)
    {
        mSeed = std::chrono::system_clock::now().
            time_since_epoch().count();
    }

    mRNG.seed(mSeed);
}

void Game::play ()
{
    mGameOver = false;

    mDisplay.reset(new Display(this));

    mLevel.reset(new Level(this));
    mLevel->generate();

    mStates.push(std::unique_ptr<GameState>(
        new GameStateStarting(this)));

    draw();
    while (!mGameOver && !mStates.empty())
    {
        auto action = processInput();

        std::visit(*this, action);

        draw();
    }
}

void Game::quit ()
{
    mGameOver = true;
}

std::ostream & Game::output ()
{
    return mOutput;
}

std::istream & Game::input ()
{
    return mInput;
}

Display * Game::display ()
{
    return mDisplay.get();
}

Level * Game::level ()
{
    return mLevel.get();
}

Prompt & Game::prompt ()
{
    return mPrompt;
}

unsigned int Game::seed ()
{
    return mSeed;
}

int Game::randomPercent ()
{
    return mPercent(mRNG);
}

std::vector<Character> & Game::characters ()
{
    return mCharacters;
}

std::vector<Character> const & Game::characters () const
{
    return mCharacters;
}

std::optional<int> Game::defaultCharacterId () const
{
    return mDefaultCharacterId;
}

void Game::setDefaultCharacterId (int id)
{
    if (findCharacter(id) == nullptr)
    {
        mDefaultCharacterId = std::nullopt;
    }
    else
    {
        mDefaultCharacterId = id;
    }
}

Character * Game::findCharacter (int id)
{
    for (auto & character: mCharacters)
    {
        if (character.id() == id)
        {
            return &character;
        }
    }

    return nullptr;
}

Character const * Game::findCharacter (int id) const
{
    for (auto & character: mCharacters)
    {
        if (character.id() == id)
        {
            return &character;
        }
    }

    return nullptr;
}

std::vector<Character> & Game::creatures ()
{
    return mCreatures;
}

std::vector<Character> const & Game::creatures () const
{
    return mCreatures;
}

Character * Game::findCreature (int id)
{
    for (auto & creature: mCreatures)
    {
        if (creature.id() == id)
        {
            return &creature;
        }
    }

    return nullptr;
}

Character const * Game::findCreature (int id) const
{
    for (auto & creature: mCreatures)
    {
        if (creature.id() == id)
        {
            return &creature;
        }
    }

    return nullptr;
}

void Game::addEvent (GameEvent const & event)
{
    mEvents.push_back(event);
}

std::vector<GameEvent> const & Game::events () const
{
    return mEvents;
}

void Game::spawnCharacters (
    std::vector<Character> const & characters)
{
    mCharacters.clear();
    mDefaultCharacterId = std::nullopt;

    for (auto const & character: characters)
    {
        mCharacters.push_back(character);

        addEvent(CharacterSpawned {character.id()});
    }

    placeCharacters();
}

void Game::placeCharacters ()
{
    if (mLevel == nullptr)
    {
        return;
    }

    auto locations = mLevel->entryLocations(mCharacters.size());

    unsigned int i = 0;
    for (auto & character: mCharacters)
    {
        character.setLocation(locations[i]);
        ++i;
    }
}

void Game::spawnCreatures ()
{
    if (mLevel == nullptr)
    {
        return;
    }

    mCreatures = mLevel->spawnCreatures();
}

void Game::operator () (GameState::Unknown & action)
{
    mDisplay->beginStreamingToDialog();

    mDisplay->dialogBuffer()
        << "Unknown command.";

    mDisplay->endStreamingToDialog();
}

void Game::operator () (GameState::Keep & action)
{
    processUpdate();
    processEvents();
}

void Game::operator () (GameState::Swap & action)
{
    processUpdate();
    processEvents();
    mStates.top() = std::move(action.newState);
}

void Game::operator () (GameState::Push & action)
{
    processUpdate();
    processEvents();
    mStates.push(std::move(action.newState));
}

void Game::operator () (GameState::Pop & action)
{
    processUpdate();
    processEvents();
    mStates.pop();
}

Behavior * Game::playerCharacterBehavior () const
{
    static BehaviorPC behavior;

    return &behavior;
}

Behavior * Game::creatureBehavior () const
{
    static BehaviorCreature behavior;

    return &behavior;
}

GameState::StateAction Game::processInput ()
{
    if (mStates.empty())
    {
        return GameState::Keep {};
    }

    return mStates.top()->processInput();
}

void Game::processUpdate ()
{
    if (mStates.empty())
    {
        return;
    }

    mStates.top()->processUpdate();
}

void Game::processEvents ()
{
    if (mStates.empty())
    {
        return;
    }

    mStates.top()->processEvents();

    mEvents.clear();
}

void Game::draw ()
{
    if (mStates.empty())
    {
        return;
    }

    mStates.top()->draw();
}
