#include "GameStateInventory.h"

#include "Game.h"

#include <algorithm>
#include <iomanip>

GameStateInventory::GameStateInventory (
    Game * game,
    int characterId)
: GameState(game), mCharacterId(characterId)
{
    initializeResources();
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
{ }

void GameStateInventory::processEvents ()
{
    for (auto const & event: mGame->events())
    {
        std::visit(*this, event);
    }
}

void GameStateInventory::draw ()
{
    auto character = mGame->findCharacter(mCharacterId);
    if (character == nullptr)
    {
        return;
    }

    std::string characterLabel = "Character ";
    characterLabel += character->symbol();

    mGame->display()->clear();

    mGame->output() << "Inventory" << std::endl;
    mGame->output() << std::endl;

    int itemId = 1;
    mGame->output() << std::setw(3) << ' '
        << std::setw(20) << characterLabel
        << std::setw(7) << "Count"
        << std::endl;
    if (mResourcesOwned.empty())
    {
        mGame->output() << std::setw(3) << ' '
            << std::setw(20) << "nothing"
            << std::endl;
    }
    else
    {
        for (auto & resource: mResourcesOwned)
        {
            mGame->output() << std::setw(3) << itemId++
                << std::setw(20) << resource.name()
                << std::setw(7) << resource.count()
                << std::endl;
        }
    }

    mGame->output() << std::endl;

    itemId = 1;
    mGame->output() << std::setw(3) << ' '
        << std::setw(20) << "Available"
        << std::setw(7) << "Count"
        << std::endl;
    if (mResourcesAvailable.empty())
    {
        mGame->output() << std::setw(3) << ' '
            << std::setw(20) << "nothing"
            << std::endl;
    }
    else
    {
        for (auto & resource: mResourcesAvailable)
        {
            mGame->output() << std::setw(3) << itemId++
                << std::setw(20) << resource.name()
                << std::setw(7) << resource.count()
                << std::endl;
        }
    }

    mGame->output() << std::endl;
    mGame->output() << "1. Done." << std::endl;
    mGame->output() << "2. Take." << std::endl;
    mGame->output() << "3. Drop." << std::endl;
    mGame->output() << std::endl;
}

void GameStateInventory::initializeResources ()
{
    auto character = mGame->findCharacter(mCharacterId);
    if (character == nullptr)
    {
        return;
    }

    // Owned resources should already have valid counts.
    mResourcesOwned = character->resources();

    auto level = mGame->level();
    auto location = character->location();

    Tile * tile = nullptr;
    if (level != nullptr)
    {
        tile = level->findTile(location);
    }
    if (tile == nullptr)
    {
        return;
    }

    // Only copy tile resources that are valid to take.
    // Make a copy of each resource so we don't affect the
    // resources in the tile.
    for (auto resource: tile->resources())
    {
        if (!resource.foundCount() ||
            resource.foundCount().value() == 0)
        {
            continue;
        }

        resource.setCount(resource.foundCount().value());
        mResourcesAvailable.push_back(resource);
    }
}

void moveResources (
    Game * game,
    std::vector<Resource> & src,
    std::vector<Resource> & dest,
    std::string const & msgSelect,
    std::string const & msgCount)
{
    int selection = 0;
    if (src.empty())
    {
        return;
    }
    else if (src.size() > 1)
    {
        selection = game->prompt().promptNumber(
            msgSelect,
            1,
            src.size());

        // Adjust selection for zero based indices.
        --selection;
    }

    int countOriginal = src[selection].count();
    int countEntered = 1;
    if (countOriginal > 3)
    {
        countEntered = game->prompt().promptNumber(
            msgCount,
            1,
            countOriginal);
    }

    std::string name = src[selection].name();
    auto matchingResource = std::find_if(
        dest.begin(),
        dest.end(),
        [name] (Resource const & item)
        {
            return item.name() == name;
        });
    if (matchingResource == dest.end())
    {
        Resource copy = src[selection];
        copy.setCount(countEntered);
        dest.push_back(copy);
    }
    else
    {
        int countCurrent = matchingResource->count();
        matchingResource->setCount(countCurrent + countEntered);
    }

    if (countEntered == countOriginal)
    {
        auto position = src.begin() + selection;
        src.erase(position);
    }
    else
    {
        int countNew = countOriginal - countEntered;
        src[selection].setCount(countNew);
    }
}

void GameStateInventory::handleTake ()
{
    moveResources(
        mGame,
        mResourcesAvailable,
        mResourcesOwned,
        "Which item do you want? ",
        "How many do you want? ");
}

void GameStateInventory::handleDrop ()
{
    moveResources(
        mGame,
        mResourcesOwned,
        mResourcesAvailable,
        "Which item do you want to drop? ",
        "How many do you want to drop? ");
}

void GameStateInventory::handleDone ()
{
    auto character = mGame->findCharacter(mCharacterId);
    if (character == nullptr)
    {
        return;
    }

    auto level = mGame->level();
    auto location = character->location();

    Tile * tile = nullptr;
    if (level != nullptr)
    {
        tile = level->findTile(location);
    }
    if (tile == nullptr)
    {
        return;
    }

    // The character resources are ready to be copied.
    character->resources() = mResourcesOwned;

    // Update the found counts with the counts before
    // copying the resources back to the tile.
    for (auto & resource: mResourcesAvailable)
    {
        resource.setFoundCount(resource.count());
    }
    tile->resources() = mResourcesAvailable;
}
