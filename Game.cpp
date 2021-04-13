#include "Game.h"

#include "ASCII.h"
#include "ComponentAbilityAdjustment.h"
#include "ComponentColor.h"
#include "ComponentConsumable.h"
#include "ComponentContainer.h"
#include "ComponentDrawable.h"
#include "ComponentEdible.h"
#include "ComponentFindable.h"
#include "ComponentGroupable.h"
#include "ComponentHealth.h"
#include "ComponentIdentifiable.h"
#include "ComponentLayer.h"
#include "ComponentLocateable.h"
#include "ComponentPhysical.h"
#include "ComponentRegistry.h"
#include "ComponentTradeable.h"
#include "GameItemRegistry.h"
#include "GameStateStarting.h"
#include "Point.h"

#include <algorithm>
#include <chrono>
#include <ctime>

Game::Game(
    std::ostream & output,
    std::istream & input,
    unsigned int seed)
: mOutput(output), mInput(input),
  mGameOver(true),
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

    registerComponents();
    registerItems();

    mDisplay.reset(new Display(this));

    mLevel.reset(new Level(this));
    mLevel->generate();

    setLayerCollisionsInLevel();

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

GameOptions & Game::options ()
{
    return mOptions;
}

std::vector<GameItem> & Game::characters ()
{
    return mCharacters;
}

std::vector<GameItem> const & Game::characters () const
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

GameItem * Game::findCharacter (int id)
{
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    if (id < ComponentIdentifiable::FirstInstanceId)
    {
        for (auto & character: mCharacters)
        {
            if (identifiable->shortcutId(&character) == id)
            {
                return &character;
            }
        }
    }
    else
    {
        for (auto & character: mCharacters)
        {
            if (identifiable->instanceId(&character) == id)
            {
                return &character;
            }
        }
    }

    return nullptr;
}

GameItem const * Game::findCharacter (int id) const
{
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    if (id < ComponentIdentifiable::FirstInstanceId)
    {
        for (auto & character: mCharacters)
        {
            if (identifiable->shortcutId(&character) == id)
            {
                return &character;
            }
        }
    }
    else
    {
        for (auto & character: mCharacters)
        {
            if (identifiable->instanceId(&character) == id)
            {
                return &character;
            }
        }
    }

    return nullptr;
}

std::vector<GameItem> & Game::creatures ()
{
    return mCreatures;
}

std::vector<GameItem> const & Game::creatures () const
{
    return mCreatures;
}

GameItem * Game::findCreature (int id)
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

GameItem const * Game::findCreature (int id) const
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
    std::vector<GameItem> const & characters)
{
    mCharacters.clear();
    mDefaultCharacterId = std::nullopt;

    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    for (auto const & character: characters)
    {
        mCharacters.push_back(character);

        addEvent(CharacterSpawned {identifiable->instanceId(&character)});
    }

    placeCharacters();
}

void Game::setLayerCollisionsInLevel ()
{
    if (mLevel == nullptr)
    {
        return;
    }

    // Get the layer ids ready.
    int animalsLayerId = 0;
    int deepWaterLayerId = 0;
    int nonPlayersLayerId = 0;
    int playersLayerId = 0;
    int solidsLayerId = 0;

    GameItem * layerItem;
    layerItem = GameItemRegistry::find("animals");
    if (layerItem != nullptr)
    {
        animalsLayerId = layerItem->id();
    }
    layerItem = GameItemRegistry::find("deep water");
    if (layerItem != nullptr)
    {
        deepWaterLayerId = layerItem->id();
    }
    layerItem = GameItemRegistry::find("non-player characters");
    if (layerItem != nullptr)
    {
        nonPlayersLayerId = layerItem->id();
    }
    layerItem = GameItemRegistry::find("player characters");
    if (layerItem != nullptr)
    {
        playersLayerId = layerItem->id();
    }
    layerItem = GameItemRegistry::find("solids");
    if (layerItem != nullptr)
    {
        solidsLayerId = layerItem->id();
    }

    mLevel->setCollidingLayerIds(
        playersLayerId,
        {
            animalsLayerId,
            deepWaterLayerId,
            nonPlayersLayerId,
            playersLayerId,
            solidsLayerId
        });

    mLevel->setCollidingLayerIds(
        nonPlayersLayerId,
        {
            animalsLayerId,
            deepWaterLayerId,
            nonPlayersLayerId,
            playersLayerId,
            solidsLayerId
        });

    mLevel->setCollidingLayerIds(
        animalsLayerId,
        {
            animalsLayerId,
            deepWaterLayerId,
            nonPlayersLayerId,
            playersLayerId,
            solidsLayerId
        });
}

void Game::placeCharacters ()
{
    if (mLevel == nullptr || mDisplay == nullptr)
    {
        return;
    }

    auto locations = mLevel->entryLocations(mCharacters.size());

    auto locateable = ComponentRegistry::find<ComponentLocateable>();

    unsigned int i = 0;
    for (auto & character: mCharacters)
    {
        locateable->setLocation(&character, locations[i]);
        ++i;
    }

    mDisplay->ensureVisibleInMap(
        locations[0],
        mLevel->width(),
        mLevel->height());
}

void Game::spawnCreatures ()
{
    if (mLevel == nullptr)
    {
        return;
    }

    mLevel->spawnCreatures();
}

void Game::addLayer (int layerId)
{
    if (std::find(mLayerIds.cbegin(), mLayerIds.cend(), layerId) ==
        mLayerIds.cend())
    {
        mLayerIds.push_back(layerId);
    }
}

void Game::removeLayer (int layerId)
{
    mLayerIds.erase(std::find(mLayerIds.cbegin(), mLayerIds.cend(), layerId));
}

std::vector<int> Game::layers () const
{
    return mLayerIds;
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

void Game::registerComponents ()
{
    ComponentRegistry::clear();

    ComponentAbilityAdjustment ability;
    ComponentColor color;
    ComponentConsumable consumable;
    ComponentContainer container;
    ComponentDrawable drawable;
    ComponentEdible edible;
    ComponentFindable findable;
    ComponentGroupable groupable;
    ComponentHealth health;
    ComponentIdentifiable identifiable;
    ComponentLayer layer;
    ComponentLocateable locateable;
    ComponentPhysical physical;
    ComponentTradeable tradeable;

    ComponentRegistry::add(ability);
    ComponentRegistry::add(color);
    ComponentRegistry::add(consumable);
    ComponentRegistry::add(container);
    ComponentRegistry::add(drawable);
    ComponentRegistry::add(edible);
    ComponentRegistry::add(findable);
    ComponentRegistry::add(groupable);
    ComponentRegistry::add(health);
    ComponentRegistry::add(identifiable);
    ComponentRegistry::add(layer);
    ComponentRegistry::add(locateable);
    ComponentRegistry::add(physical);
    ComponentRegistry::add(tradeable);
}

void Game::registerItems ()
{
    GameItemRegistry::clear();

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    auto groupable = ComponentRegistry::find<ComponentGroupable>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    auto layer = ComponentRegistry::find<ComponentLayer>();
    auto color = ComponentRegistry::find<ComponentColor>();

    // Layers
    auto gameItem = GameItemRegistry::add("deep water");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "deep water");
    addLayer(gameItem->id());
    int deepWaterLayerId = gameItem->id();

    gameItem = GameItemRegistry::add("shallow water");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "shallow water");
    addLayer(gameItem->id());
    int shallowWaterLayerId = gameItem->id();

    gameItem = GameItemRegistry::add("land");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "land");
    addLayer(gameItem->id());
    int landLayerId = gameItem->id();

    gameItem = GameItemRegistry::add("solids");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "solids");
    addLayer(gameItem->id());
    int solidsLayerId = gameItem->id();

    gameItem = GameItemRegistry::add("player characters");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "player characters");
    addLayer(gameItem->id());

    gameItem = GameItemRegistry::add("non-player characters");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "non-player characters");
    addLayer(gameItem->id());

    gameItem = GameItemRegistry::add("animals");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "animals");
    addLayer(gameItem->id());

    gameItem = GameItemRegistry::add("flying");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "flying");
    addLayer(gameItem->id());

    // Tiles
    gameItem = GameItemRegistry::add("deep water tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "deep water");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, 'w');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, deepWaterLayerId);
    gameItem->addComponent(color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::BackBlue});

    gameItem = GameItemRegistry::add("shallow water tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "shallow water");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, 'w');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, shallowWaterLayerId);
    gameItem->addComponent(color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::BackBlue});

    gameItem = GameItemRegistry::add("dirt tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "dirt");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, ' ');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(color->id());

    gameItem = GameItemRegistry::add("sand tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "sand");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, 's');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::ForeYellow});

    gameItem = GameItemRegistry::add("marsh tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "marsh");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, 'm');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::ForeBlue});

    gameItem = GameItemRegistry::add("rock tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "rock");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, ' ');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(color->id());

    gameItem = GameItemRegistry::add("ice tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "ice");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, 'i');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::ForeBrightWhite});

    gameItem = GameItemRegistry::add("tree tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "tree");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, 't');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::ForeGreen});

    gameItem = GameItemRegistry::add("grass tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "grass");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, ' ');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(color->id());

    gameItem = GameItemRegistry::add("bush tile");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "bush");
    gameItem->addComponent(drawable->id());
    drawable->setSymbol(gameItem, 'b');
    gameItem->addComponent(layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(color->id());

    // Actors
    gameItem = GameItemRegistry::add("character");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "character");

    gameItem = GameItemRegistry::add("rat");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "rat");

    // Items
    gameItem = GameItemRegistry::add("gold");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "gold");

    gameItem = GameItemRegistry::add("silver");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "silver");

    gameItem = GameItemRegistry::add("red apple");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "red apple");

    gameItem = GameItemRegistry::add("torch");
    gameItem->addComponent(identifiable->id());
    identifiable->setName(gameItem, "torch");
    gameItem->addComponent(groupable->id());
    groupable->setPropertyNames(gameItem, {"percentageRemainingEstimate"});
}
