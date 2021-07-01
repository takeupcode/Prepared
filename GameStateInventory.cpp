#include "GameStateInventory.h"

#include "ComponentDrawable.h"
#include "ComponentGroupable.h"
#include "ComponentIdentifiable.h"
#include "ComponentLocation.h"
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
    mCharacter = mGame->findItem(characterId);
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

    auto location = ComponentRegistry::find<ComponentLocation>();
    Point2i point = location->location(mCharacter);

    if (level != nullptr)
    {
        mTile = level->findTile(point);
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

void GameStateInventory::processUpdate ()
{
}

void GameStateInventory::processEvents ()
{
    for (auto const & event: mGame->events())
    {
        std::visit(*this, event);
    }
}

std::string GameStateInventory::formatItemCount (
    DisplayGroup & displayGroup) const
{
    return "(" + std::to_string(displayGroup.count) + ")";
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

    unsigned int row = 1;
    unsigned int i = 0;
    for (; i < smallest; ++i, ++row)
    {
        auto & itemOwnedName = itemsOwned[i].name;
        auto itemOwnedCountStr = formatItemCount(itemsOwned[i]);

        auto & itemAvailableName = itemsAvailable[i].name;
        auto itemAvailableCountStr = formatItemCount(itemsAvailable[i]);

        output << std::setw(5) << row
            << std::setw(20) << itemOwnedName
            << std::setw(10) << itemOwnedCountStr
            << std::setw(20) << itemAvailableName
            << std::setw(10) << itemAvailableCountStr
            << std::endl;
    }

    for (; i < itemsOwned.size(); ++i, ++row)
    {
        auto & itemName = itemsOwned[i].name;
        auto itemCountStr = formatItemCount(itemsOwned[i]);
        output << std::setw(5) << row
            << std::setw(20) << itemName
            << std::setw(10) << itemCountStr
            << std::endl;
    }

    for (; i < itemsAvailable.size(); ++i, ++row)
    {
        auto & itemName = itemsAvailable[i].name;
        auto itemCountStr = formatItemCount(itemsAvailable[i]);

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
    std::vector<int> const & source,
    std::vector<DisplayGroup> & destination) const
{
    for (auto const instanceId: source)
    {
        auto gameItem = mGame->findItem(instanceId);
        if (gameItem == nullptr)
        {
            continue;
        }

        auto registeredId = gameItem->id();
        auto const & name = identifiable->name(gameItem);
        auto count = identifiable->count(gameItem);

        DisplayItem newItem {
            registeredId,
            name,
            count,
            instanceId
        };
        addToDisplayGroupCollection (
            groupable,
            identifiable,
            newItem,
            destination);
    }
}

void GameStateInventory::addToDisplayGroupCollection (
    ComponentGroupable * groupable,
    ComponentIdentifiable * identifiable,
    DisplayGroup const & source,
    std::vector<DisplayGroup> & destination) const
{
    for (auto const instanceId: source.items)
    {
        auto gameItem = mGame->findItem(instanceId);
        if (gameItem == nullptr)
        {
            continue;
        }

        DisplayItem newItem {
            source.registeredId,
            source.name,
            source.items.size() == 1 ? source.count : 1,
            instanceId
        };
        addToDisplayGroupCollection (
            groupable,
            identifiable,
            newItem,
            destination);
    }
}

void GameStateInventory::addToDisplayGroupCollection (
    ComponentGroupable * groupable,
    ComponentIdentifiable * identifiable,
    DisplayItem const & source,
    std::vector<DisplayGroup> & destination) const
{
    auto gameItem = mGame->findItem(source.instanceId);
    if (gameItem == nullptr)
    {
        return;
    }

    auto registeredId = source.registeredId;
    auto groupIter = std::find_if(
        destination.begin(), destination.end(),
        [registeredId] (auto const & group)
        { return group.registeredId == registeredId; });
    if (groupIter != destination.end())
    {
        auto groupedItem = mGame->findItem(groupIter->items[0]);
        if (groupedItem == nullptr)
        {
            return;
        }

        auto countable = identifiable->isCountable(groupedItem);
        if (countable)
        {
            // For countable items, all we need to do is update
            // the temp count. There will only be a single item
            // but we only update its count if the user accepts
            // the changes.
            groupIter->count += source.count;
        }
        else
        {
            // For individual items, we need to check if the
            // current item belongs in the group or not.
            // We only need to check against the first item in
            // the group because all the items belong to the
            // same group.
            if (groupable->areGroupedTogether(gameItem, groupedItem))
            {
                ++groupIter->count;
                groupIter->items.push_back(source.instanceId);
            }
        }
        return;
    }

    // Add a completely new group if either the game item registered
    // id is not yet in the collection, or if it is but the game item
    // doesn't belong in the existing group.
    DisplayGroup newGroup {
        registeredId,
        source.name,
        source.count};
    newGroup.items.push_back(source.instanceId);
    destination.push_back(std::move(newGroup));
}

GameStateInventory::DisplayGroup
GameStateInventory::removeFromDisplayGroupCollection (
    ComponentIdentifiable * identifiable,
    std::vector<DisplayGroup> & source,
    unsigned int index,
    unsigned int count) const
{
    DisplayGroup result {
        source[index].registeredId,
        source[index].name,
        0 // This will be set later once instance ids are also added.
    };

    if (source[index].items.empty())
    {
        return result;
    }

    auto itemsIter = source[index].items.begin();
    auto firstInstanceId = *itemsIter;
    auto firstGameItem = mGame->findItem(firstInstanceId);
    if (firstGameItem == nullptr)
    {
        return result;
    }

    // First handle the case where the single item uses a count.
    auto countable = identifiable->isCountable(firstGameItem);
    if (countable)
    {
        int instanceId = firstInstanceId;
        if (count >= source[index].count)
        {
            // Take all the game item.
            count = source[index].count;
            source.erase(source.begin() + index);
        }
        else
        {
            // Take some of the game item.
            auto clonedItem = mGame->cloneItem(instanceId);
            if (clonedItem == nullptr)
            {
                return result;
            }
            source[index].count -= count;
            instanceId = clonedItem->instanceId();
        }

        result.items.push_back(instanceId);
        result.count = count;

        return result;
    }

    // Otherwise, handle the case where the game items are stored
    // individually.
    if (count > source[index].items.size())
    {
        count = static_cast<unsigned int>(source[index].items.size());
    }
    unsigned int remainingCount = count;
    while (remainingCount != 0)
    {
        result.items.push_back(*itemsIter);
        ++result.count;

        source[index].items.erase(itemsIter);
        source[index].count -= 1;
        itemsIter = source[index].items.begin();

        --remainingCount;
    }

    if (itemsIter == source[index].items.end())
    {
        source.erase(source.begin() + index);
    }

    return result;
}

std::vector<int> GameStateInventory::displayGroupCollectionToGameItems (
    ComponentIdentifiable * identifiable,
    std::vector<DisplayGroup> const & source) const
{
    std::vector<int> result;

    for (auto const & displayGroup: source)
    {
        if (displayGroup.items.empty())
        {
            continue;
        }

        auto itemsIter = displayGroup.items.begin();
        auto firstInstanceId = *itemsIter;
        auto firstGameItem = mGame->findItem(firstInstanceId);
        if (firstGameItem == nullptr)
        {
            return result;
        }

        // First handle the case where the single item uses a count.
        auto countable = identifiable->isCountable(firstGameItem);
        if (countable)
        {
            identifiable->setCount(firstGameItem, displayGroup.count);
            result.push_back(firstInstanceId);

            continue;
        }

        // Otherwise, handle the case where the game items are stored
        // individually.
        while (itemsIter != displayGroup.items.end())
        {
            result.push_back(*itemsIter);
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
    unsigned int listSize = static_cast<unsigned int>(source.size());
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
    int quantityAvailable = source[row - 1].count;
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

    mCharacter->setItems(displayGroupCollectionToGameItems(
        identifiable,
        mItemsOwned));
    mTile->setItems(displayGroupCollectionToGameItems(
        identifiable,
        mItemsAvailable));
}
