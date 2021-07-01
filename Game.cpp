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
#include "ComponentLocation.h"
#include "ComponentPhysical.h"
#include "ComponentRegistry.h"
#include "ComponentTradeable.h"
#include "Constants.h"
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
: mOutput(output),
  mInput(input),
  mGameOver(true),
  mPrompt(output, input),
  mOriginalSeed(seed),
  mPercent(0, 100)
{ }

void Game::play ()
{
    reset(mOriginalSeed);

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

std::optional<int> Game::defaultCharacterId () const
{
    return mDefaultCharacterId;
}

void Game::setDefaultCharacterId (int instanceId)
{
    if (findItem(instanceId) == nullptr)
    {
        mDefaultCharacterId = std::nullopt;
    }
    else
    {
        mDefaultCharacterId = instanceId;
    }
}

GameItem * Game::createItem (int id)
{
    auto gameItem = std::unique_ptr<GameItem>(new GameItem(id));
    int instanceId = gameItem->instanceId();
    GameItem * result = mGameItems.try_emplace(
        instanceId,
        std::move(gameItem)).
        first->second.get();

    for (auto const & tag: result->tags())
    {
        if (mGameItemIndices.find(tag) != mGameItemIndices.end())
        {
            mGameItemIndicesMap[tag].insert(instanceId);
        }
    }

    return result;
}

GameItem * Game::cloneItem (int instanceId)
{
    auto item = findItem(instanceId);
    if (item == nullptr)
    {
        return nullptr;
    }

    int id = item->id();
    auto gameItem = std::unique_ptr<GameItem>(new GameItem(id));
    int newInstanceId = gameItem->instanceId();
    GameItem * result = mGameItems.try_emplace(
        newInstanceId,
        std::move(gameItem)).
        first->second.get();

    *result = *item;

    for (auto const & tag: result->tags())
    {
        if (mGameItemIndices.find(tag) != mGameItemIndices.end())
        {
            mGameItemIndicesMap[tag].insert(instanceId);
        }
    }

    return result;
}

void Game::eraseItem (int instanceId)
{
    auto item = findItem(instanceId);
    if (item == nullptr)
    {
        return;
    }

    for (auto const & tag: item->tags())
    {
        if (mGameItemIndices.find(tag) != mGameItemIndices.end())
        {
            mGameItemIndicesMap[tag].erase(instanceId);
        }
    }

    mGameItems.erase(instanceId);
}

GameItem * Game::findItem (int instanceId) const
{
    if (instanceId < GameItem::FirstInstanceId)
    {
        auto shortcutResult = mGameItemShortcutMap.find(instanceId);
        if (shortcutResult == mGameItemShortcutMap.end())
        {
            return nullptr;
        }

        instanceId = shortcutResult->second;
    }

    auto gameItemsMapResult = mGameItems.find(instanceId);
    if (gameItemsMapResult == mGameItems.end())
    {
        return nullptr;
    }

    return gameItemsMapResult->second.get();
}

std::vector<GameItem *> Game::findItems (
    std::string const & tag) const
{
    std::vector<GameItem *> result;

    if (mGameItemIndices.find(tag) != mGameItemIndices.end())
    {
        auto iter = mGameItemIndicesMap.at(tag).cbegin();
        while (iter != mGameItemIndicesMap.at(tag).cend())
        {
            auto item = findItem(*iter);
            if (item != nullptr)
            {
                result.push_back(item);
            }
            ++iter;
        }
    }

    return result;
}

void Game::addEvent (GameEvent const & event)
{
    mEvents.push_back(event);
}

std::vector<GameEvent> const & Game::events () const
{
    return mEvents;
}

void Game::spawnCharacters ()
{
    mDefaultCharacterId = std::nullopt;

    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    auto characters = findItems(TAGS::PC);
    for (auto const & character: characters)
    {
        int instanceId = character->instanceId();
        int shortcutId = identifiable->shortcutId(character);
        mGameItemShortcutMap.try_emplace(shortcutId, instanceId);

        addEvent(GameItemSpawned {});
    }

    if (mLevel == nullptr || mDisplay == nullptr)
    {
        return;
    }

    unsigned int characterCount = static_cast<unsigned int>(
        characters.size());
    auto locations = mLevel->entryLocations(characterCount);

    auto location = ComponentRegistry::find<ComponentLocation>();

    unsigned int i = 0;
    for (auto character: characters)
    {
        location->setLocation(character, locations[i]);
        ++i;
    }

    mDisplay->ensureVisibleInMap(
        locations[0],
        mLevel->width(),
        mLevel->height());
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

void Game::operator () (TagAdded const & event)
{
    if (mGameItemIndices.find(event.tag) != mGameItemIndices.end())
    {
        GameItem * item = findItem(event.itemInstanceId);
        if (item == nullptr)
        {
            return;
        }

        mGameItemIndicesMap[event.tag].insert(event.itemInstanceId);
    }
}

void Game::operator () (TagRemoved const & event)
{
    if (mGameItemIndices.find(event.tag) != mGameItemIndices.end())
    {
        mGameItemIndicesMap[event.tag].extract(event.itemInstanceId);
    }
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
    for (auto const & event: mEvents)
    {
        std::visit(*this, event);
    }

    if (!mStates.empty())
    {
        mStates.top()->processEvents();
    }

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
    ComponentLocation location;
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
    ComponentRegistry::add(location);
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
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "deep water");
    addLayer(gameItem->id());
    int deepWaterLayerId = gameItem->id();

    gameItem = GameItemRegistry::add("shallow water");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "shallow water");
    addLayer(gameItem->id());
    int shallowWaterLayerId = gameItem->id();

    gameItem = GameItemRegistry::add("land");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "land");
    addLayer(gameItem->id());
    int landLayerId = gameItem->id();

    gameItem = GameItemRegistry::add("solids");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "solids");
    addLayer(gameItem->id());

    gameItem = GameItemRegistry::add("player characters");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "player characters");
    addLayer(gameItem->id());

    gameItem = GameItemRegistry::add("non-player characters");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "non-player characters");
    addLayer(gameItem->id());

    gameItem = GameItemRegistry::add("animals");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "animals");
    addLayer(gameItem->id());

    gameItem = GameItemRegistry::add("flying");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "flying");
    addLayer(gameItem->id());

    // Tiles
    gameItem = GameItemRegistry::add("deep water tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "deep water");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, 'w');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, deepWaterLayerId);
    gameItem->addComponent(this, color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::BackBlue});

    gameItem = GameItemRegistry::add("shallow water tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "shallow water");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, 'w');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, shallowWaterLayerId);
    gameItem->addComponent(this, color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::BackBlue});

    gameItem = GameItemRegistry::add("dirt tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "dirt");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, ' ');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(this, color->id());

    gameItem = GameItemRegistry::add("sand tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "sand");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, 's');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(this, color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::ForeYellow});

    gameItem = GameItemRegistry::add("marsh tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "marsh");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, 'm');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(this, color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::ForeBlue});

    gameItem = GameItemRegistry::add("rock tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "rock");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, ' ');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(this, color->id());

    gameItem = GameItemRegistry::add("ice tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "ice");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, 'i');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(this, color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::ForeBrightWhite});

    gameItem = GameItemRegistry::add("tree tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "tree");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, 't');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(this, color->id());
    color->setAttributesBasic(gameItem,
        {ASCIIGraphic::ForeGreen});

    gameItem = GameItemRegistry::add("grass tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "grass");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, ' ');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(this, color->id());

    gameItem = GameItemRegistry::add("bush tile");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "bush");
    gameItem->addComponent(this, drawable->id());
    drawable->setSymbol(gameItem, 'b');
    gameItem->addComponent(this, layer->id());
    layer->setLayerId(gameItem, landLayerId);
    gameItem->addComponent(this, color->id());

    // Actors
    gameItem = GameItemRegistry::add("character");
    gameItem->addTag(this, TAGS::PC);
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "character");

    gameItem = GameItemRegistry::add("rat");
    gameItem->addTag(this, TAGS::NPC);
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "rat");

    // Items
    gameItem = GameItemRegistry::add("gold");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "gold");
    identifiable->setIsCountable(gameItem, true);

    gameItem = GameItemRegistry::add("silver");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "silver");
    identifiable->setIsCountable(gameItem, true);

    gameItem = GameItemRegistry::add("red apple");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "red apple");

    gameItem = GameItemRegistry::add("torch");
    gameItem->addComponent(this, identifiable->id());
    identifiable->setName(gameItem, "torch");
    gameItem->addComponent(this, groupable->id());
    groupable->setPropertyNames(gameItem, {"percentageRemainingEstimate"});
}

void Game::reset (unsigned int seed)
{
    if (seed == 0)
    {
        seed = static_cast<int>(std::chrono::system_clock::now().
            time_since_epoch().count());
    }
    mSeed = seed;
    mRNG.seed(mSeed);

    mGameOver = false;
    mDefaultCharacterId = std::nullopt;
    mEvents.clear();
    mLayerIds.clear();
    while (!mStates.empty())
    {
        mStates.pop();
    }

    mGameItems.clear();
    mGameItemIndices.clear();
    mGameItemIndicesMap.clear();
    mGameItemShortcutMap.clear();

    createGameItemIndex(TAGS::PC);
    createGameItemIndex(TAGS::NPC);

    mDisplay.reset(nullptr);
    mLevel.reset(nullptr);
}

void Game::createGameItemIndex (std::string const & name)
{
    if (mGameItemIndices.find(name) == mGameItemIndices.end())
    {
        mGameItemIndices.insert(name);

        GameItemIndex emptyIndex;
        mGameItemIndicesMap.insert({name, emptyIndex});
    }
}
