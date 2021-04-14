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
        GameItem gameItem(registeredGameItem->id());

        gameItem.addComponent(identifiable->id());

        identifiable->setCount(&gameItem, 10);

        mCharacters.back().items().push_back(gameItem);
    }

    registeredGameItem = GameItemRegistry::find("torch");
    if (registeredGameItem != nullptr)
    {
        GameItem gameItem(registeredGameItem->id());

        gameItem.addComponent(identifiable->id());
        gameItem.addComponent(consumable->id());

        identifiable->setUniqueInstanceId(&gameItem);
        consumable->setPercentageRemaining(&gameItem, 100.0);

        mCharacters.back().items().push_back(gameItem);

        identifiable->setUniqueInstanceId(&gameItem);

        mCharacters.back().items().push_back(gameItem);
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

    character.addComponent(drawable->id());
    drawable->setSymbol(&character, symbol);

    character.addComponent(health->id());
    health->setMaxHealth(&character, 60);
    health->setHealth(&character, 60);

    character.addComponent(identifiable->id());
    identifiable->setUniqueInstanceId(&character);
    identifiable->setShortcutId(&character, static_cast<int>(mCharacters.size()) + 1);

    int playersLayerId = 0;
    GameItem * layerItem;
    layerItem = GameItemRegistry::find("player characters");
    if (layerItem != nullptr)
    {
        playersLayerId = layerItem->id();
    }

    character.addComponent(layer->id());
    layer->setLayerId(&character, playersLayerId);

    character.addComponent(location->id());
    // The actual location will be set later.

    return character;
}
