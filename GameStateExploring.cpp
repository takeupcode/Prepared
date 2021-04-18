#include "GameStateExploring.h"

#include "CommandAttack.h"
#include "CommandInventory.h"
#include "CommandMove.h"
#include "CommandOptions.h"
#include "CommandQuit.h"
#include "CommandSelectCharacter.h"
#include "CommandTurn.h"
#include "CommandWait.h"
#include "ComponentDrawable.h"
#include "ComponentFindable.h"
#include "ComponentHealth.h"
#include "ComponentIdentifiable.h"
#include "ComponentLocation.h"
#include "ComponentRegistry.h"
#include "ComponentTradeable.h"
#include "Game.h"
#include "GameItem.h"
#include "GameItemRegistry.h"

#include <algorithm>

GameStateExploring::GameStateExploring (Game * game)
: GameState(game)
{ }

GameState::StateAction GameStateExploring::processInput ()
{
    static std::string lastInputString = "";
    std::string inputString = mGame->prompt().promptText(
        "Enter command: ", true);
    if (inputString.empty() && !lastInputString.empty())
    {
        inputString = lastInputString;
    }
    lastInputString = inputString;

    std::unique_ptr<Command> command;

    command = CommandMove::tryCreate(inputString);
    if (command != nullptr)
    {
        return command->execute(mGame);
    }

    command = CommandSelectCharacter::tryCreate(inputString);
    if (command != nullptr)
    {
        return command->execute(mGame);
    }

    command = CommandTurn::tryCreate(inputString);
    if (command != nullptr)
    {
        return command->execute(mGame);
    }

    command = CommandAttack::tryCreate(inputString);
    if (command != nullptr)
    {
        return command->execute(mGame);
    }

    command = CommandInventory::tryCreate(inputString);
    if (command != nullptr)
    {
        return command->execute(mGame);
    }

    command = CommandWait::tryCreate(inputString);
    if (command != nullptr)
    {
        return command->execute(mGame);
    }

    command = CommandOptions::tryCreate(inputString);
    if (command != nullptr)
    {
        return command->execute(mGame);
    }

    command = CommandQuit::tryCreate(inputString);
    if (command != nullptr)
    {
        return command->execute(mGame);
    }

    return GameState::Unknown {};
}

void GameStateExploring::processUpdate ()
{
}

void GameStateExploring::processEvents ()
{
    for (auto const & event: mGame->events())
    {
        std::visit(*this, event);
    }
}

void GameStateExploring::draw ()
{
    auto display = mGame->display();
    display->clear();

    mGame->output() << "-------------------------------" << std::endl;

    mGame->level()->draw();

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    for (auto const & item: mGame->items())
    {
        if (item.hasTag("pc") || item.hasTag("npc"))
        {
            drawable->draw(&item, display);
        }
    }

    display->update();
}

void GameStateExploring::operator () (
    GameItemMoved const & moved) const
{
    auto display = mGame->display();
    auto character = mGame->findItem(
        moved.itemInstanceId);

    if (character == nullptr)
    {
        return;
    }

    display->beginStreamingToDialog();

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    char symbol = drawable->symbol(character);
    display->dialogBuffer()
        << "Character " << symbol << " moved.";

    auto location = ComponentRegistry::find<ComponentLocation>();
    auto tile = mGame->level()->findTile(
        location->location(character));
    if (tile != nullptr)
    {
        auto findable = ComponentRegistry::find<ComponentFindable>();
        auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
        auto tradeable = ComponentRegistry::find<ComponentTradeable>();
        for (auto & item: tile->items())
        {
            auto foundCountValid = findable->foundCountValid(&item);
            auto foundCount = findable->foundCount(&item);
            if (!foundCountValid)
            {
                auto targetCount = findable->targetCount(&item);
                auto chanceOfFinding = findable->chanceOfFinding(&item);
                auto percent = mGame->randomPercent();
                if (percent <= chanceOfFinding)
                {
                    if (targetCount == 1)
                    {
                        foundCount = 1;
                    }
                    else
                    {
                        foundCount = targetCount * percent / 100;
                    }
                }
                else
                {
                    foundCount = 0;
                }
                findable->setFoundCount(&item, foundCount);
            }

            auto itemName = identifiable->name(&item);
            if (itemName.empty())
            {
                continue;
            }

            auto value = tradeable->value(&item);

            display->dialogBuffer()
                << " And found "
                << foundCount
                << " " << itemName
                << " worth " << value
                << ((foundCount > 1) ? " each." : ".");
        }
    }

    display->endStreamingToDialog();
}

void GameStateExploring::operator () (
    GameItemDamaged const & damaged) const
{
    int instanceId = damaged.itemInstanceId;
    int damage = damaged.damage;
    auto display = mGame->display();
    auto item = mGame->findItem(instanceId);

    if (item == nullptr)
    {
        return;
    }

    display->beginStreamingToDialog();

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    if (item->hasTag("pc"))
    {
        display->dialogBuffer()
            << "Character " << drawable->symbol(item)
            << " hit with " << damage << " damage.";

        auto health = ComponentRegistry::find<ComponentHealth>();
        if (health->isDead(item))
        {
            display->dialogBuffer() << " And died.";
        }
    }
    else if (item->hasTag("npc"))
    {
        display->dialogBuffer()
            << "Creature hit with " << damage << " damage.";

        auto health = ComponentRegistry::find<ComponentHealth>();
        if (health->isDead(item))
        {
            display->dialogBuffer() << " And died.";

            mGame->items().erase(
                std::find_if(
                    mGame->items().begin(),
                    mGame->items().end(),
                    [instanceId] (auto const & creature)
                    {
                        return creature.instanceId() == instanceId;
                    }
                ));
        }
    }

    display->endStreamingToDialog();
}
