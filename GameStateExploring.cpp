#include "GameStateExploring.h"

#include "CommandAttack.h"
#include "CommandInventory.h"
#include "CommandMove.h"
#include "CommandQuit.h"
#include "CommandSelectCharacter.h"
#include "CommandTurn.h"
#include "CommandWait.h"
#include "ComponentDrawable.h"
#include "ComponentIdentifiable.h"
#include "ComponentRegistry.h"
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

    command = CommandQuit::tryCreate(inputString);
    if (command != nullptr)
    {
        return command->execute(mGame);
    }

    return GameState::Unknown {};
}

void GameStateExploring::processUpdate ()
{
    for (auto & character: mGame->characters())
    {
        character.update(mGame);
    }

    for (auto & creature: mGame->creatures())
    {
        creature.update(mGame);
    }
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

    mGame->output() << "----------------" << std::endl;

    mGame->level()->draw();

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    for (auto const & character: mGame->characters())
    {
        drawable->draw(&character, display);
    }

    for (auto const & creature: mGame->creatures())
    {
        drawable->draw(&creature, display);
    }

    display->update();
}

void GameStateExploring::operator () (
    CharacterMoved const & characterMoved) const
{
    auto display = mGame->display();
    auto character = mGame->findCharacter(
        characterMoved.characterId);

    if (character == nullptr)
    {
        return;
    }

    display->beginStreamingToDialog();

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    char symbol = drawable->symbol(character);
    display->dialogBuffer()
        << "Character " << symbol << " moved.";

    auto tile = mGame->level()->findTile(
        character->location());
    if (tile != nullptr)
    {
        auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
        for (auto & item: tile->items())
        {
            auto foundCount = identifiable->foundCount(&item);
            if (!foundCount)
            {
                foundCount = identifiable->possibleCount(&item);
                identifiable->setFoundCount(&item, foundCount);
            }

            auto itemName = identifiable->name(&item);
            if (itemName.empty())
            {
                continue;
            }

            auto value = identifiable->value(&item);

            display->dialogBuffer()
                << " And found "
                << foundCount.value()
                << " " << itemName
                << " worth " << value << " each.";
        }
    }

    display->endStreamingToDialog();
}

void GameStateExploring::operator () (
    CharacterHit const & characterHit) const
{
    int characterId = characterHit.characterId;
    int damage = characterHit.damage;
    auto display = mGame->display();
    auto character = mGame->findCharacter(characterId);

    if (character == nullptr)
    {
        return;
    }

    display->beginStreamingToDialog();

    display->dialogBuffer()
        << "Character " << character->symbol()
        << " hit with " << damage << " damage.";

    if (character->isDead())
    {
        display->dialogBuffer() << " And died.";
    }

    display->endStreamingToDialog();
}

void GameStateExploring::operator () (
    CreatureHit const & creatureHit) const
{
    int creatureId = creatureHit.creatureId;
    int damage = creatureHit.damage;
    auto display = mGame->display();
    auto creature = mGame->findCreature(creatureId);

    if (creature == nullptr)
    {
        return;
    }

    display->beginStreamingToDialog();

    display->dialogBuffer()
        << "Creature hit with " << damage << " damage.";

    if (creature->isDead())
    {
        display->dialogBuffer() << " And died.";

        mGame->creatures().erase(
            std::find_if(
                mGame->creatures().begin(),
                mGame->creatures().end(),
                [creatureId] (auto const & creature)
                {
                    return creature.id() == creatureId;
                }
            ));
    }

    display->endStreamingToDialog();
}
