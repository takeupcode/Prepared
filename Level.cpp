#include "Level.h"

#include "ASCII.h"
#include "ComponentColor.h"
#include "ComponentConsumable.h"
#include "ComponentDrawable.h"
#include "ComponentIdentifiable.h"
#include "ComponentLayer.h"
#include "ComponentLocateable.h"
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
    auto marshTile = GameItemRegistry::find("marsh tile");
    auto iceTile = GameItemRegistry::find("ice tile");
    auto treeTile = GameItemRegistry::find("tree tile");
    auto grassTile = GameItemRegistry::find("grass tile");

    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    mTiles.clear();

    mHeight = static_cast<int>(terrain.size());
    mWidth = static_cast<int>(terrain[0].size());
    for (int y = 0; y < mHeight; ++y)
    {
        for (int x = 0; x < mWidth; ++x)
        {
            switch (terrain[y][x])
            {
                case GameMap::Terrain::Water:
                {
                    GameItem waterInst(deepTile->id());
                    waterInst.addComponent(identifiable->id());
                    identifiable->setUniqueInstanceId(&waterInst);
                    mTiles.push_back(waterInst);
                    break;
                }

                case GameMap::Terrain::Sand:
                {
                    GameItem sandInst(sandTile->id());
                    sandInst.addComponent(identifiable->id());
                    identifiable->setUniqueInstanceId(&sandInst);
                    mTiles.push_back(sandInst);
                    break;
                }

                case GameMap::Terrain::Marsh:
                {
                    GameItem marshInst(marshTile->id());
                    marshInst.addComponent(identifiable->id());
                    identifiable->setUniqueInstanceId(&marshInst);
                    mTiles.push_back(marshInst);
                    break;
                }

                case GameMap::Terrain::Trees:
                {
                    GameItem dirtTreeInst(dirtTile->id());
                    dirtTreeInst.addComponent(identifiable->id());
                    identifiable->setUniqueInstanceId(&dirtTreeInst);
                    mTiles.push_back(dirtTreeInst);

                    GameItem treeInst(treeTile->id());
                    treeInst.addComponent(identifiable->id());
                    identifiable->setUniqueInstanceId(&treeInst);
                    mLayeredTiles.try_emplace(
                        identifiable->instanceId(&dirtTreeInst),
                        treeInst);
                    break;
                }

                case GameMap::Terrain::Grass:
                {
                    GameItem dirtGrassInst(dirtTile->id());
                    dirtGrassInst.addComponent(identifiable->id());
                    identifiable->setUniqueInstanceId(&dirtGrassInst);
                    mTiles.push_back(dirtGrassInst);

                    GameItem grassInst(grassTile->id());
                    grassInst.addComponent(identifiable->id());
                    identifiable->setUniqueInstanceId(&grassInst);
                    mLayeredTiles.try_emplace(
                        identifiable->instanceId(&dirtGrassInst),
                        grassInst);
                    break;
                }

                case GameMap::Terrain::Ice:
                {
                    GameItem iceInst(iceTile->id());
                    iceInst.addComponent(identifiable->id());
                    identifiable->setUniqueInstanceId(&iceInst);
                    mTiles.push_back(iceInst);
                    break;
                }
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

void Level::spawnCreatures () const
{
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    auto locateable = ComponentRegistry::find<ComponentLocateable>();

    for (int i = 0; i < 10; ++i)
    {
        auto creature = createRat();

        auto location = findRandomLocationOnLand();
        locateable->setLocation(&creature, location);

        mGame->addEvent(CreatureSpawned {identifiable->instanceId(&creature)});
    }
}

Point Level::calculateMoveLocation (
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

GameItem const * Level::findTile (Point const & location) const
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

GameItem Level::createRat () const
{
    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    auto layer = ComponentRegistry::find<ComponentLayer>();
    auto locateable = ComponentRegistry::find<ComponentLocateable>();

    auto registeredItem = GameItemRegistry::find("rat");
    GameItem rat(registeredItem->id());

    rat.addComponent(drawable->id());
    drawable->setSymbol(&rat, 'a');

    rat.addComponent(identifiable->id());
    identifiable->setUniqueInstanceId(&rat);

    int animalsLayerId = 0;
    GameItem * layerItem;
    layerItem = GameItemRegistry::find("animals");
    if (layerItem != nullptr)
    {
        animalsLayerId = layerItem->id();
    }

    rat.addComponent(layer->id());
    layer->setLayerId(&rat, animalsLayerId);

    rat.addComponent(locateable->id());
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
