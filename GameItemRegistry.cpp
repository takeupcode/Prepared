#include "GameItemRegistry.h"

#include "GameItem.h"

int GameItemRegistry::mNextId = 1;
GameItemRegistry::PermanentIdMap GameItemRegistry::mGameItemIds;
GameItemRegistry::GameItemMap GameItemRegistry::mGameItems;

GameItem * GameItemRegistry::add (std::string const & permanentId)
{
    auto gameItem = find(permanentId);
    if (gameItem == nullptr)
    {
        mGameItemIds.try_emplace(permanentId, mNextId);
        mGameItems.try_emplace(
            mNextId,
            std::unique_ptr<GameItem>(new GameItem(mNextId, 0)));

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
