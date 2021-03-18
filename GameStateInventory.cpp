#include "GameStateInventory.h"

#include "ComponentDrawable.h"
#include "ComponentGroupable.h"
#include "ComponentIdentifiable.h"
#include "ComponentMoveable.h"
#include "ComponentRegistry.h"
#include "Game.h"
#include "GameItem.h"
#include "GameItemRegistry.h"

#include <algorithm>
#include <iomanip>

GameStateInventory::GameStateInventory (
    Game * game,
    int characterId)
: GameState(game), mCharacter(nullptr), mTile(nullptr)
{
    mCharacter = mGame->findCharacter(characterId);
    if (mCharacter == nullptr)
    {
        return;
    }

    auto groupable = ComponentRegistry::find<ComponentGroupable>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    addToDisplayGroupCollection (
        groupable,
        identifiable,
        mCharacter->items(),
        mItemsOwned);

    auto level = mGame->level();

    auto moveable = ComponentRegistry::find<ComponentMoveable>();
    Point location = moveable->location(mCharacter);

    if (level != nullptr)
    {
        mTile = level->findTile(location);
        if (mTile != nullptr)
        {
            addToDisplayGroupCollection (
                groupable,
                identifiable,
                mTile->items(),
                mItemsAvailable);
        }
    }
}

GameState::StateAction GameStateInventory::processInput ()
{
    int inputNumber = mGame->prompt().promptNumber("Enter choice: ");
    switch (inputNumber)
    {
    case 1:
        handleDone();
        return GameState::Pop {};

    case 2:
        handleTake();
        break;

    case 3:
        handleDrop();
        break;
    }

    return GameState::Keep {};
}

void GameStateInventory::processEvents ()
{
    for (auto const & event: mGame->events())
    {
        std::visit(*this, event);
    }
}

std::string GameStateInventory::formatItemCount (
    DisplayGroup * displayGroup) const
{
    return "(" + std::to_string(displayGroup->total) + ")";
}

void GameStateInventory::displayItems (
    std::ostream & output,
    std::string const & characterLabel,
    std::vector<DisplayGroup> & itemsOwned,
    std::vector<DisplayGroup> & itemsAvailable) const
{
    output << std::setw(35) << characterLabel
        << std::setw(30) << "Available" << std::endl;

    auto smallest = std::min(itemsOwned.size(), itemsAvailable.size());

    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    unsigned int row = 1;
    unsigned int i = 0;
    for (; i < smallest; ++i, ++row)
    {
        auto itemOwnedName = identifiable->name(&itemsOwned[i].items[0]);
        auto itemOwnedCountStr = formatItemCount(&itemsOwned[i]);

        auto itemAvailableName = identifiable->name(&itemsAvailable[i].items[0]);
        auto itemAvailableCountStr = formatItemCount(&itemsAvailable[i]);

        output << std::setw(5) << row
            << std::setw(20) << itemOwnedName
            << std::setw(10) << itemOwnedCountStr
            << std::setw(20) << itemAvailableName
            << std::setw(10) << itemAvailableCountStr
            << std::endl;
    }

    for (; i < itemsOwned.size(); ++i, ++row)
    {
        std::string itemName = identifiable->name(&itemsOwned[i].items[0]);
        auto itemCountStr = formatItemCount(&itemsOwned[i]);
        output << std::setw(5) << row
            << std::setw(20) << itemName
            << std::setw(10) << itemCountStr
            << std::endl;
    }

    for (; i < itemsAvailable.size(); ++i, ++row)
    {
        std::string itemName = identifiable->name(&itemsAvailable[i].items[0]);
        auto itemCountStr = formatItemCount(&itemsAvailable[i]);

        output << std::setw(5) << row
            << std::setw(50) << itemName
            << std::setw(10) << itemCountStr
            << std::endl;
    }
}

void GameStateInventory::draw ()
{
    if (mCharacter == nullptr)
    {
        return;
    }

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    char symbol = drawable->symbol(mCharacter);

    std::string characterLabel = "Character ";
    characterLabel += symbol;

    mGame->display()->clear();

    mGame->output() << "Take items" << std::endl;
    mGame->output() << std::endl;
    displayItems(mGame->output(),
        characterLabel,
        mItemsOwned,
        mItemsAvailable);

    mGame->output() << std::endl;
    mGame->output() << "1. Done." << std::endl;
    mGame->output() << "2. Take." << std::endl;
    mGame->output() << "3. Drop." << std::endl;
    mGame->output() << std::endl;
}

void GameStateInventory::addToDisplayGroupCollection (
    ComponentGroupable * groupable,
    ComponentIdentifiable * identifiable,
    std::vector<GameItem> const & source,
    std::vector<DisplayGroup> & destination) const
{
    for (auto const & gameItem: source)
    {
        auto registeredId = gameItem.id();
        auto instanceId = identifiable->instanceId(&gameItem);
        auto count = identifiable->count(&gameItem);

        auto groupIter = std::find_if(
            destination.begin(), destination.end(),
            [registeredId] (auto const & group)
            { return group.registeredId == registeredId; });
        if (groupIter != destination.end())
        {
            if (instanceId == 0)
            {
                // For countable items, all we need to do is update the
                // total count and the item count to be the same. There
                // will only be a single item which is why we use items[0].
                groupIter->total += count;
                identifiable->setCount(
                    &(groupIter->items[0]),
                    groupIter->total);

                continue;
            }
            else
            {
                // For individual items, we need to check if the current
                // item belongs in the group or not. We only need to check
                // against the first item in the group because all the items
                // in the group also belong to the same group.
                if (groupable->areGroupedTogether(&gameItem, &(groupIter->items[0])))
                {
                    ++groupIter->total;
                    groupIter->items.push_back(gameItem);

                    continue;
                }
            }
        }

        // Add a completely new group if either the game item registered id
        // is not yet in the collection, or if it is but the game item doesn't
        // belong in the existing group.
        DisplayGroup newGroup {registeredId, count};
        newGroup.items.push_back(gameItem);
        destination.push_back(std::move(newGroup));
    }
}

std::vector<GameItem> GameStateInventory::removeFromDisplayGroupCollection (
    ComponentIdentifiable * identifiable,
    std::vector<DisplayGroup> & source,
    unsigned int index,
    unsigned int count) const
{
    std::vector<GameItem> result;

    if (count > source[index].total)
    {
        count = source[index].total;
    }

    auto itemsIter = source[index].items.begin();
    if (itemsIter == source[index].items.end())
    {
        return result;
    }

    auto & firstGameItem = *itemsIter;
    auto firstInstanceId = identifiable->instanceId(&firstGameItem);

    // First handle the case where the single item uses a count.
    if (firstInstanceId == 0)
    {
        GameItem copiedFirstGameItem(firstGameItem);
        auto itemCount = identifiable->count(&firstGameItem);
        unsigned int takeCount = 0;
        if (count >= itemCount)
        {
            // Take all the game item.
            takeCount = itemCount;
            source.erase(source.begin() + index);
        }
        else
        {
            // Take some of the game item.
            takeCount = count;
            identifiable->setCount(&firstGameItem, itemCount - takeCount);
            source[index].total = itemCount - takeCount;
        }

        identifiable->setCount(&copiedFirstGameItem, takeCount);
        result.push_back(std::move(copiedFirstGameItem));

        return result;
    }

    // Otherwise, handle the case where the game items are stored
    // individually.
    unsigned int remainingCount = count;
    while (remainingCount != 0)
    {
        if (itemsIter == source[index].items.end())
        {
            // This should not happen unless the total is greater
            // than the sum of each item count.
            break;
        }

        auto & gameItem = *itemsIter;
        GameItem copiedGameItem(gameItem);

        source[index].items.erase(itemsIter);
        source[index].total -= 1;
        itemsIter = source[index].items.begin();

        result.push_back(std::move(copiedGameItem));
        --remainingCount;
    }

    if (itemsIter == source[index].items.end())
    {
        source.erase(source.begin() + index);
    }

    return result;
}

std::vector<GameItem> GameStateInventory::displayGroupCollectionToGameItems (
    ComponentIdentifiable * identifiable,
    std::vector<DisplayGroup> const & source) const
{
    std::vector<GameItem> result;

    for (auto const & displayGroup: source)
    {
        auto itemsIter = displayGroup.items.begin();
        if (itemsIter == displayGroup.items.end())
        {
            continue;
        }

        auto & firstGameItem = *itemsIter;
        auto firstInstanceId = identifiable->instanceId(&firstGameItem);

        // First handle the case where the single item uses a count.
        if (firstInstanceId == 0)
        {
            GameItem copiedFirstGameItem(firstGameItem);
            result.push_back(std::move(copiedFirstGameItem));

            continue;
        }

        // Otherwise, handle the case where the game items are stored
        // individually.
        while (itemsIter != displayGroup.items.end())
        {
            auto & gameItem = *itemsIter;
            GameItem copiedGameItem(gameItem);
            result.push_back(std::move(copiedGameItem));
            ++itemsIter;
        }
    }

    return result;
}

void GameStateInventory::transferItems (
    std::vector<DisplayGroup> & source,
    std::vector<DisplayGroup> & destination,
    unsigned int index,
    unsigned int count) const
{
    auto groupable = ComponentRegistry::find<ComponentGroupable>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    auto transferredItems = removeFromDisplayGroupCollection(
        identifiable,
        source,
        index,
        count);

    addToDisplayGroupCollection(
        groupable,
        identifiable,
        transferredItems,
        destination);
}

void GameStateInventory::handleTransfer (
    Game * game,
    std::string const & promptMessage,
    std::vector<DisplayGroup> & source,
    std::vector<DisplayGroup> & destination) const
{
    unsigned int listSize = source.size();
    if (listSize == 0)
    {
        return;
    }

    unsigned int row = 1;
    if (listSize > 1)
    {
        row = game->prompt().promptNumber(
            promptMessage,
            1,
            listSize);
    }

    int quantityDesired = 1;
    int quantityAvailable = source[row - 1].total;
    if (quantityAvailable > 1)
    {
        quantityDesired = game->prompt().promptNumber(
            "How many? ",
            1,
            quantityAvailable);
    }

    transferItems(source, destination, row - 1, quantityDesired);
}

void GameStateInventory::handleTake ()
{
    handleTransfer(
        mGame,
        "What do you want to take? ",
        mItemsAvailable,
        mItemsOwned);
}

void GameStateInventory::handleDrop ()
{
    handleTransfer(
        mGame,
        "What do you want to drop? ",
        mItemsOwned,
        mItemsAvailable);
}

void GameStateInventory::handleDone ()
{
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    mCharacter->items() = displayGroupCollectionToGameItems(
        identifiable, mItemsOwned);
    mTile->items() = displayGroupCollectionToGameItems(
        identifiable, mItemsAvailable);
}
