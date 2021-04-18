#include "GameItemRegistry.h"

#include "GameItem.h"

int GameItemRegistry::mNextId = 1;
std::unordered_map<std::string, int> GameItemRegistry::mGameItemIds;
std::unordered_map<int, std::unique_ptr<GameItem>> GameItemRegistry::mGameItems;

GameItem * GameItemRegistry::add (std::string const & permanentId)
{
    auto gameItem = find(permanentId);
    if (gameItem == nullptr)
    {
        mGameItemIds.try_emplace(permanentId, mNextId);
        mGameItems.try_emplace(
            mNextId,
            std::make_unique<GameItem>(mNextId, 0));

        gameItem = find(mNextId);
        ++mNextId;
    }

    return gameItem;
}

GameItem * GameItemRegistry::find (int id)
{
    auto gameItemsMapResult = mGameItems.find(id);
    if (gameItemsMapResult == mGameItems.end())
    {
        return nullptr;
    }

    return gameItemsMapResult->second.get();
}

GameItem * GameItemRegistry::find (std::string const & permanentId)
{
    auto gameItemIdsMapResult = mGameItemIds.find(permanentId);
    if (gameItemIdsMapResult == mGameItemIds.end())
    {
        return nullptr;
    }

    return find(gameItemIdsMapResult->second);
}

void GameItemRegistry::clear ()
{
    mGameItemIds.clear();
    mGameItems.clear();
}
