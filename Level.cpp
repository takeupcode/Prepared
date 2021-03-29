#include "Level.h"

#include "ComponentConsumable.h"
#include "ComponentDrawable.h"
#include "ComponentIdentifiable.h"
#include "ComponentLayer.h"
#include "ComponentRegistry.h"
#include "Display.h"
#include "Game.h"
#include "GameItemRegistry.h"
#include "GameMap.h"

#include <algorithm>
#include <random>
#include <string>

Level::Level (Game * game, int seed)
: mGame(game), mSeed(seed), mWidth(0), mHeight(0)
{
    if (mSeed == 0)
    {
        mSeed = std::random_device()();
    }
}

void Level::generate ()
{
    auto terrain = GameMap::create();

    // Get the layer ids ready.
    int deepWaterLayerId = 0;
    int landLayerId = 0;
    int solidsLayerId = 0;

    GameItem * layerItem;
    layerItem = GameItemRegistry::find("deep water");
    if (layerItem != nullptr)
    {
        deepWaterLayerId = layerItem->id();
    }
    layerItem = GameItemRegistry::find("land");
    if (layerItem != nullptr)
    {
        landLayerId = layerItem->id();
    }
    layerItem = GameItemRegistry::find("solids");
    if (layerItem != nullptr)
    {
        solidsLayerId = layerItem->id();
    }

    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    mTiles.clear();

    mHeight = static_cast<int>(terrain.size());
    mWidth = static_cast<int>(terrain[0].size());
    for (int y = 0; y < mHeight; ++y)
    {
        for (int x = 0; x < mWidth; ++x)
        {
            char baseSymbol = ' ';
            int baseLayerId = 0;
            char layeredSymbol = ' ';
            int layeredLayerId = 0;
            switch (terrain[y][x])
            {
                case GameMap::Terrain::Water:
                    baseSymbol = 'w';
                    baseLayerId = deepWaterLayerId;
                    break;

                case GameMap::Terrain::Sand:
                    baseSymbol = 's';
                    baseLayerId = landLayerId;
                    break;

                case GameMap::Terrain::Marsh:
                    baseSymbol = 'm';
                    baseLayerId = landLayerId;
                    break;

                case GameMap::Terrain::Trees:
                    baseLayerId = landLayerId;
                    layeredSymbol = 't';
                    layeredLayerId = solidsLayerId;
                    break;

                case GameMap::Terrain::Grass:
                    baseLayerId = landLayerId;
                    layeredLayerId = landLayerId;
                    break;

                case GameMap::Terrain::Ice:
                    baseSymbol = 'i';
                    baseLayerId = landLayerId;
                    break;
            }
            auto tile = createTile(baseSymbol, baseLayerId);
            mTiles.push_back(tile);

            if (layeredLayerId != 0)
            {
                auto tileInstanceId = identifiable->instanceId(&tile);
                auto layeredTile = createTile(layeredSymbol, layeredLayerId);
                mLayeredTiles.try_emplace(tileInstanceId, layeredTile);
            }
        }
    }

    mEntryLocations.push_back(findRandomLocationOnLand());

    auto consumable = ComponentRegistry::find<ComponentConsumable>();

    Point location(0, 0);
    int index;

    auto registeredGameItem = GameItemRegistry::find("gold");
    if (registeredGameItem != nullptr)
    {
        GameItem gameItem(registeredGameItem->id());
        gameItem.addComponent(identifiable->id());

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        identifiable->setCount(&gameItem, 30);
        mTiles[index].items().push_back(gameItem);

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        identifiable->setCount(&gameItem, 20);
        mTiles[index].items().push_back(gameItem);
    }

    registeredGameItem = GameItemRegistry::find("silver");
    if (registeredGameItem != nullptr)
    {
        GameItem gameItem(registeredGameItem->id());
        gameItem.addComponent(identifiable->id());

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        identifiable->setCount(&gameItem, 10);
        mTiles[index].items().push_back(gameItem);
    }

    registeredGameItem = GameItemRegistry::find("red apple");
    if (registeredGameItem != nullptr)
    {
        GameItem gameItem(registeredGameItem->id());
        gameItem.addComponent(identifiable->id());

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        identifiable->setCount(&gameItem, 2);
        mTiles[index].items().push_back(gameItem);
    }

    registeredGameItem = GameItemRegistry::find("torch");
    if (registeredGameItem != nullptr)
    {
        GameItem gameItem(registeredGameItem->id());
        gameItem.addComponent(identifiable->id());
        gameItem.addComponent(consumable->id());

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        identifiable->setUniqueInstanceId(&gameItem);
        consumable->setPercentageRemaining(&gameItem, 55.0);
        mTiles[index].items().push_back(gameItem);
    }
}

void Level::draw () const
{
    auto display = mGame->display();
    auto mapOrigin = display->mapScrollOrigin();

    auto drawable = ComponentRegistry::find<ComponentDrawable>();

    display->beginStreamingToMap();

    for (int y = mapOrigin.y;
         y < mapOrigin.y + Display::MapDisplayHeight;
         ++y)
    {
        for (int x = mapOrigin.x;
             x < mapOrigin.x + Display::MapDisplayWidth;
             ++x)
        {
            auto tile = findTile({x, y});
            display->mapBuffer() << ' '
                << drawable->symbol(tile);
        }

        display->mapBuffer() << std::endl;
    }

    display->endStreamingToMap();
}

std::vector<Point> Level::entryLocations (unsigned int count) const
{
    std::vector<Point> locations;

    for (unsigned int i = 0; i < count; ++i)
    {
        if (mEntryLocations.empty())
        {
            locations.push_back({0, 0});
        }
        else
        {
            locations.push_back(mEntryLocations[0]);
        }
    }

    return locations;
}

std::vector<Character> Level::spawnCreatures () const
{
    std::vector<Character> creatures;

    Behavior * behavior = mGame->creatureBehavior();
    creatures.push_back(Character(100, 'a', behavior));
    creatures.back().setLocation({7, 3});
    creatures.back().setMaxHealth(10, false);
    creatures.back().setCurrentHealth(10, false);
    creatures.back().setAttackDamage(4);

    creatures.push_back(Character(101, 'a', behavior));
    creatures.back().setLocation({6, 7});
    creatures.back().setMaxHealth(10, false);
    creatures.back().setCurrentHealth(10, false);
    creatures.back().setAttackDamage(4);

    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    auto locateable = ComponentRegistry::find<ComponentLocateable>();

    for (int i = 0; i < 10; ++i)
    {
        auto creature = createRat();

        auto location = mLevel->findRandomLocationOnLand();
        locateable->setLocation(&creature, location);

        addEvent(CreatureSpawned {identifiable->instanceId(&creature)});
    }

    return creatures;
}

Location Level::calculateMoveLocation (
    Point const & current,
    Point const & proposed,
    int moveLayerId) const
{
    if (proposed.x < 0 ||
        static_cast<unsigned int>(proposed.x) >= mWidth ||
        proposed.y < 0 ||
        static_cast<unsigned int>(proposed.y) >= mHeight
    )
    {
        return current;
    }

    if (mCollidingLayerIds.find(moveLayerId) ==
        mCollidingLayerIds.end())
    {
        // The move layer has no collisions.
        return proposed;
    }

    auto proposedTile = findTile(proposed);
    if (proposedTile == nullptr)
    {
        return current;
    }

    auto layer = ComponentRegistry::find<ComponentLayer>();
    auto proposedLayerId = layer->layerId(proposedTile);

    auto const & collidingLayers = mCollidingLayerIds.at(moveLayerId);
    if (std::find(
        collidingLayers.cbegin(),
        collidingLayers.cend(),
        proposedLayerId) != collidingLayers.cend())
    {
        return current;
    }

    return proposed;
}

GameItem * Level::findTile (Point const & location)
{
    if (location.x < 0 ||
        static_cast<unsigned int>(location.x) >= mWidth ||
        location.y < 0 ||
        static_cast<unsigned int>(location.y) >= mHeight
        )
    {
        return nullptr;
    }

    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    auto & baseTile =
        mTiles[pointToRowMajorIndex(location, mWidth)];
    auto baseInstanceId = identifiable->instanceId(&baseTile);

    auto layerIter = mLayeredTiles.find(baseInstanceId);
    if (layerIter != mLayeredTiles.end())
    {
        return &mLayeredTiles.at(baseInstanceId);
    }

    return &baseTile;
}

Tile const * Level::findTile (Location const & location) const
{
    if (location.x < 0 ||
        static_cast<unsigned int>(location.x) >= mWidth ||
        location.y < 0 ||
        static_cast<unsigned int>(location.y) >= mHeight
        )
    {
        return nullptr;
    }

    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    auto & baseTile =
        mTiles[pointToRowMajorIndex(location, mWidth)];
    auto baseInstanceId = identifiable->instanceId(&baseTile);

    auto layerIter = mLayeredTiles.find(baseInstanceId);
    if (layerIter != mLayeredTiles.end())
    {
        return &mLayeredTiles.at(baseInstanceId);
    }

    return &baseTile;
}

GameItem Level::createTile (char symbol, int layerId) const
{
    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    auto layer = ComponentRegistry::find<ComponentLayer>();

    auto registeredTile = GameItemRegistry::find("tile");
    GameItem tile(registeredTile->id());

    tile.addComponent(drawable->id());
    drawable->setSymbol(&tile, symbol);

    tile.addComponent(identifiable->id());
    identifiable->setUniqueInstanceId(&tile);

    tile.addComponent(layer->id());
    layer->setLayerId(&tile, layerId);

    return tile;
}

int Level::width ()
{
    return mWidth;
}

int Level::height ()
{
    return mHeight;
}

std::vector<int> Level::collidingLayerIds (int layerId)
{
    return mCollidingLayerIds[layerId];
}

void Level::setCollidingLayerIds (
    int layerId,
    std::vector<int> const & collidingLayers)
{
    mCollidingLayerIds[layerId] = collidingLayers;
}

Point Level::findRandomLocationOnLand () const
{
    int minSize = std::min(mWidth, mHeight);

    std::mt19937 rng;
    rng.seed(mSeed);
    // An int distribution seems to be more reliable.
    std::uniform_int_distribution<std::mt19937::result_type> dist(
        1,
        minSize - 2); // Allow at least a pixel on either side.

    Point result(dist(rng), dist(rng));

    int landLayerId = 0;
    GameItem * layerItem = GameItemRegistry::find("land");
    if (layerItem != nullptr)
    {
        landLayerId = layerItem->id();
    }

    auto layer = ComponentRegistry::find<ComponentLayer>();
    while (true)
    {
        auto tile = findTile(result);
        int layerId = layer->layerId(tile);
        if (layerId == landLayerId)
        {
            break;
        }

        result.x = dist(rng);
        result.y = dist(rng);
    }

    return result;
}
