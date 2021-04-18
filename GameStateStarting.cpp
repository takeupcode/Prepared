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
{
    mCharacters.push_back(createCharacter('1'));

    auto consumable = ComponentRegistry::find<ComponentConsumable>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();

    auto registeredGameItem = GameItemRegistry::find("gold");
    if (registeredGameItem != nullptr)
    {
        GameItem gold(registeredGameItem->id());

        gold.addComponent(mGame, identifiable->id());

        identifiable->setCount(&gold, 10);

        mCharacters.back().items().push_back(gold);
    }

    registeredGameItem = GameItemRegistry::find("torch");
    if (registeredGameItem != nullptr)
    {
        GameItem torch1(registeredGameItem->id());

        torch1.addComponent(mGame, consumable->id());
        consumable->setPercentageRemaining(&torch1, 100.0);

        mCharacters.back().items().push_back(torch1);

        GameItem torch2(registeredGameItem->id());

        torch2.addComponent(mGame, consumable->id());
        consumable->setPercentageRemaining(&torch2, 100.0);

        mCharacters.back().items().push_back(torch2);
    }
}

GameState::StateAction GameStateStarting::processInput ()
{
    int inputNumber = mGame->prompt().promptNumber(
        "Enter choice: ");

    switch (inputNumber)
    {
    case 1:
        mGame->spawnCharacters(mCharacters);
        mGame->setDefaultCharacterId(1);
        mGame->spawnCreatures();
        return GameState::Swap {std::unique_ptr<GameState>(
            new GameStateExploring(mGame))};

    case 2:
        return GameState::Push {std::unique_ptr<GameState>(
            new GameStateOptions(mGame))};
        break;

    case 3:
        std::string inputString = mGame->prompt().promptText(
            "Enter symbol for new character: ");
        mCharacters.push_back(createCharacter(
            inputString[0]));
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

    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    for (auto const & character: mCharacters)
    {
        mGame->output()
            << std::setw(10) << identifiable->shortcutId(&character)
            << std::setw(10) << drawable->symbol(&character)
            << std::endl;
    }

    mGame->output() << std::endl;
    mGame->output() << "1. Play game." << std::endl;
    mGame->output() << "2. Options." << std::endl;
    mGame->output() << "3. Add character." << std::endl;
    mGame->output() << std::endl;
}

GameItem GameStateStarting::createCharacter (char symbol) const
{
    auto drawable = ComponentRegistry::find<ComponentDrawable>();
    auto health = ComponentRegistry::find<ComponentHealth>();
    auto identifiable = ComponentRegistry::find<ComponentIdentifiable>();
    auto layer = ComponentRegistry::find<ComponentLayer>();
    auto location = ComponentRegistry::find<ComponentLocation>();

    auto registeredCharacter = GameItemRegistry::find("character");
    GameItem character(registeredCharacter->id());

    character.addComponent(mGame, drawable->id());
    drawable->setSymbol(&character, symbol);

    character.addComponent(mGame, health->id());
    health->setMaxHealth(&character, 60);
    health->setHealth(&character, 60);

    character.addComponent(mGame, identifiable->id());
    identifiable->setShortcutId(&character, static_cast<int>(mCharacters.size()) + 1);

    int playersLayerId = 0;
    GameItem * layerItem;
    layerItem = GameItemRegistry::find("player characters");
    if (layerItem != nullptr)
    {
        playersLayerId = layerItem->id();
    }

    character.addComponent(mGame, layer->id());
    layer->setLayerId(&character, playersLayerId);

    character.addComponent(mGame, location->id());
    // The actual location will be set later.

    return character;
}
