#include "Level.h"

#include "ASCII.h"
#include "ComponentColor.h"
#include "ComponentConsumable.h"
#include "ComponentDrawable.h"
#include "ComponentIdentifiable.h"
#include "ComponentLayer.h"
#include "ComponentLocation.h"
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

    auto deepTile = GameItemRegistry::find("deep water tile");
    auto dirtTile = GameItemRegistry::find("dirt tile");
    auto sandTile = GameItemRegistry::find("sand tile");
    auto iceTile = GameItemRegistry::find("ice tile");
    auto treeTile = GameItemRegistry::find("tree tile");
    auto deadTreeTile = GameItemRegistry::find("dead tree tile");
    auto grassTile = GameItemRegistry::find("grass tile");

    mTiles.clear();
    mEntryLocations.clear();
    mLayeredTiles.clear();
    mCollidingLayerIds.clear();

    mHeight = static_cast<int>(terrain.size());
    mWidth = static_cast<int>(terrain[0].size());
    GameItem * tile;
    GameItem * layeredTile;
    for (int y = 0; y < mHeight; ++y)
    {
        for (int x = 0; x < mWidth; ++x)
        {
            switch (terrain[y][x])
            {
            case GameMap::Terrain::Water:
                tile = mGame->createItem(deepTile->id());
                mTiles.push_back(tile);
                break;

            case GameMap::Terrain::Dirt:
                tile = mGame->createItem(dirtTile->id());
                mTiles.push_back(tile);
                break;

            case GameMap::Terrain::Sand:
                tile = mGame->createItem(sandTile->id());
                mTiles.push_back(tile);
                break;

            case GameMap::Terrain::Tree:
                tile = mGame->createItem(dirtTile->id());
                mTiles.push_back(tile);
                layeredTile = mGame->createItem(treeTile->id());
                mLayeredTiles.try_emplace(
                    tile->instanceId(),
                    layeredTile);
                break;

            case GameMap::Terrain::DeadTree:
                tile = mGame->createItem(dirtTile->id());
                mTiles.push_back(tile);
                layeredTile = mGame->createItem(deadTreeTile->id());
                mLayeredTiles.try_emplace(
                    tile->instanceId(),
                    layeredTile);
                break;

            case GameMap::Terrain::Grass:
                tile = mGame->createItem(dirtTile->id());
                mTiles.push_back(tile);
                layeredTile = mGame->createItem(grassTile->id());
                mLayeredTiles.try_emplace(
                    tile->instanceId(),
                    layeredTile);
                break;

            case GameMap::Terrain::Ice:
                tile = mGame->createItem(iceTile->id());
                mTiles.push_back(tile);
                break;
            }
        }
    }

    mEntryLocations.push_back(findRandomLocationOnLand());

    auto consumable = ComponentRegistry::find<ComponentConsumable>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    Point2i location(0, 0);
    int index;
    GameItem * resource;

    auto registeredGameItem = GameItemRegistry::find("gold");
    if (registeredGameItem != nullptr)
    {
        resource = mGame->createItem(registeredGameItem->id());
        resource->addComponent(mGame, identifiable->id());

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        identifiable->setCount(resource, 30);
        mTiles[index]->addItem(resource->instanceId());

        resource = mGame->createItem(registeredGameItem->id());
        resource->addComponent(mGame, identifiable->id());

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        identifiable->setCount(resource, 20);
        mTiles[index]->addItem(resource->instanceId());
    }

    registeredGameItem = GameItemRegistry::find("silver");
    if (registeredGameItem != nullptr)
    {
        resource = mGame->createItem(registeredGameItem->id());
        resource->addComponent(mGame, identifiable->id());

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        identifiable->setCount(resource, 10);
        mTiles[index]->addItem(resource->instanceId());
    }

    registeredGameItem = GameItemRegistry::find("red apple");
    if (registeredGameItem != nullptr)
    {
        resource = mGame->createItem(registeredGameItem->id());
        resource->addComponent(mGame, identifiable->id());

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        identifiable->setCount(resource, 2);
        mTiles[index]->addItem(resource->instanceId());
    }

    registeredGameItem = GameItemRegistry::find("torch");
    if (registeredGameItem != nullptr)
    {
        resource = mGame->createItem(registeredGameItem->id());
        resource->addComponent(mGame, consumable->id());

        location = findRandomLocationOnLand();
        index = pointToRowMajorIndex(location, mWidth);
        consumable->setPercentageRemaining(resource, 55.0);
        mTiles[index]->addItem(resource->instanceId());
    }
}

void Level::draw () const
{
    auto display = mGame->display();
    auto mapOrigin = display->mapScrollOrigin();

    auto color = ComponentRegistry::find<ComponentColor>();
    auto drawable = ComponentRegistry::find<ComponentDrawable>();

    for (int y = mapOrigin.y;
         y < mapOrigin.y + Display::MapDisplayHeight;
         ++y)
    {
        for (int x = mapOrigin.x;
             x < mapOrigin.x + Display::MapDisplayWidth;
             ++x)
        {
            auto tile = findTile({x, y});
            display->setMapSymbol(
                ASCIIEscape::graphicSequence(color->attributesBasic(tile)) +
                    drawable->symbol(tile),
                {x, y});
        }
    }
}

std::vector<Point2i> Level::entryLocations (unsigned int count) const
{
    std::vector<Point2i> locations;

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

void Level::spawnCreatures () const
{
    auto location = ComponentRegistry::find<ComponentLocation>();

    for (int i = 0; i < 10; ++i)
    {
        auto creature = createRat();

        auto point = findRandomLocationOnLand();
        location->setLocation(creature, point);

        mGame->addEvent(GameItemSpawned {creature->instanceId()});
    }
}

Point2i Level::calculateMoveLocation (
    Point2i const & current,
    Point2i const & proposed,
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

GameItem * Level::findTile (Point2i const & location) const
{
    if (location.x < 0 ||
        static_cast<unsigned int>(location.x) >= mWidth ||
        location.y < 0 ||
        static_cast<unsigned int>(location.y) >= mHeight
        )
    {
        return nullptr;
    }

    auto baseTile =
        mTiles[pointToRowMajorIndex(location, mWidth)];
    auto baseInstanceId = baseTile->instanceId();

    auto layerIter = mLayeredTiles.find(baseInstanceId);
    if (layerIter != mLayeredTiles.end())
    {
        return mLayeredTiles.at(baseInstanceId);
    }

    return baseTile;
}

GameItem * Level::createRat () const
{
    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    auto layer = ComponentRegistry::find<ComponentLayer>();
    auto location = ComponentRegistry::find<ComponentLocation>();

    auto registeredItem = GameItemRegistry::find("rat");
    GameItem * rat = mGame->createItem(registeredItem->id());

    rat->addComponent(mGame, drawable->id());
    drawable->setSymbol(rat, 'a');

    int animalsLayerId = 0;
    GameItem * layerItem;
    layerItem = GameItemRegistry::find("animals");
    if (layerItem != nullptr)
    {
        animalsLayerId = layerItem->id();
    }

    rat->addComponent(mGame, layer->id());
    layer->setLayerId(rat, animalsLayerId);

    rat->addComponent(mGame, location->id());
    // The actual location will be set later.

    return rat;
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

Point2i Level::findRandomLocationOnLand () const
{
    int minSize = std::min(mWidth, mHeight);

    std::mt19937 rng;
    rng.seed(mSeed);
    // An int distribution seems to be more reliable.
    std::uniform_int_distribution<std::mt19937::result_type> dist(
        1,
        minSize - 2); // Allow at least a pixel on either side.

    Point2i result(dist(rng), dist(rng));

    auto layer = ComponentRegistry::find<ComponentLayer>();
    int landLayerId = 0;
    GameItem * grass = GameItemRegistry::find("grass tile");
    if (grass != nullptr)
    {
        landLayerId = layer->layerId(grass);
    }

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
