#include "GameStateStarting.h"

#include "ComponentConsumable.h"
#include "ComponentDrawable.h"
#include "ComponentHealth.h"
#include "ComponentIdentifiable.h"
#include "ComponentLayer.h"
#include "ComponentLocation.h"
#include "ComponentRegistry.h"
#include "Game.h"
#include "GameItemRegistry.h"
#include "GameStateExploring.h"
#include "GameStateOptions.h"

#include <iomanip>

GameStateStarting::GameStateStarting (Game * game)
: GameState(game)
{ }

GameState::StateAction GameStateStarting::processInput ()
{
    int inputNumber = mGame->prompt().promptNumber(
        "Enter choice: ");
    std::string inputString;

    switch (inputNumber)
    {
    case 1:
        if (mCharacterSymbols.empty())
        {
            inputString = mGame->prompt().promptText(
                "Enter symbol for character: ");
            mCharacterSymbols.push_back(inputString[0]);
        }

        createCharacters();
        mGame->spawnCharacters();
        mGame->setDefaultCharacterId(1);
        mGame->spawnCreatures();
        return GameState::Swap {std::unique_ptr<GameState>(
            new GameStateExploring(mGame))};

    case 2:
        return GameState::Push {std::unique_ptr<GameState>(
            new GameStateOptions(mGame))};
        break;

    case 3:
        inputString = mGame->prompt().promptText(
            "Enter symbol for new character: ");
        mCharacterSymbols.push_back(inputString[0]);
        break;
    }

    return GameState::Keep {};
}

void GameStateStarting::processUpdate ()
{ }

void GameStateStarting::processEvents ()
{
    for (auto const & event: mGame->events())
    {
        std::visit(*this, event);
    }
}

void GameStateStarting::draw ()
{
    mGame->display()->clear();

    mGame->output() << "Welcome to Prepared" << std::endl;
    mGame->output() << std::endl;

    mGame->output() << std::setw(10) << "Character"
        << std::setw(10) << "Symbol" << std::endl;

    int i = 0;
    for (auto const & character: mCharacterSymbols)
    {
        mGame->output()
            << std::setw(10) << ++i
            << std::setw(10) << character
            << std::endl;
    }

    mGame->output() << std::endl;
    mGame->output() << "1. Play game." << std::endl;
    mGame->output() << "2. Options." << std::endl;
    mGame->output() << "3. Add character." << std::endl;
    mGame->output() << std::endl;
}

void GameStateStarting::createCharacters ()
{
    auto consumable = ComponentRegistry::find<ComponentConsumable>();
    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    auto health = ComponentRegistry::find<ComponentHealth>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    auto layer = ComponentRegistry::find<ComponentLayer>();
    auto location = ComponentRegistry::find<ComponentLocation>();

    auto registeredCharacter = GameItemRegistry::find("character");

    int playersLayerId = 0;
    GameItem * layerItem;
    layerItem = GameItemRegistry::find("player characters");
    if (layerItem != nullptr)
    {
        playersLayerId = layerItem->id();
    }

    int i = 0;
    for (auto symbol: mCharacterSymbols)
    {
        auto character = mGame->createItem(registeredCharacter->id());

        character->addComponent(mGame, drawable->id());
        drawable->setSymbol(character, symbol);

        character->addComponent(mGame, health->id());
        health->setMaxHealth(character, 60);
        health->setHealth(character, 60);

        character->addComponent(mGame, identifiable->id());
        identifiable->setShortcutId(character, ++i);

        character->addComponent(mGame, layer->id());
        layer->setLayerId(character, playersLayerId);

        character->addComponent(mGame, location->id());
        // The actual location will be set later.

        GameItem * resource;
        auto registeredGameItem = GameItemRegistry::find("gold");
        if (registeredGameItem != nullptr)
        {
            resource = mGame->createItem(registeredGameItem->id());
            resource->addComponent(mGame, identifiable->id());
            identifiable->setCount(resource, 150);
            character->addItem(resource->instanceId());
        }

        registeredGameItem = GameItemRegistry::find("torch");
        if (registeredGameItem != nullptr)
        {
            resource = mGame->createItem(registeredGameItem->id());
            resource->addComponent(mGame, consumable->id());
            consumable->setPercentageRemaining(resource, 100.0);
            character->addItem(resource->instanceId());

            resource = mGame->createItem(registeredGameItem->id());
            resource->addComponent(mGame, consumable->id());
            consumable->setPercentageRemaining(resource, 100.0);
            character->addItem(resource->instanceId());
        }
    }
}
