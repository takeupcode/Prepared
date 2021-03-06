#ifndef GAMESTATEINVENTORY_H
#define GAMESTATEINVENTORY_H

#include "GameState.h"

#include <string>
#include <vector>

class ComponentGroupable;
class ComponentIdentifiable;
class GameItem;

class GameStateInventory : public GameState
{
public:
    GameStateInventory (
        Game * game,
        int characterId);

    StateAction processInput () override;

    void processUpdate () override;

    void processEvents () override;

    void draw () override;

    template <typename EventVariant>
    void operator () (EventVariant const &) const
    { }

private:
    struct DisplayGroup
    {
        int registeredId;
        std::string name; // Store name so it's readily available.
        unsigned int count; // Only modify item counts when done.
        std::vector<int> items;
    };
    struct DisplayItem
    {
        int registeredId;
        std::string name; // Store name so it's readily available.
        unsigned int count; // Only modify item counts when done.
        int instanceId;
    };

    std::string formatItemCount (DisplayGroup & displayGroup) const;

    void displayItems (
        std::ostream & output,
        std::string const & characterLabel,
        std::vector<DisplayGroup> & itemsOwned,
        std::vector<DisplayGroup> & itemsAvailable) const;

    void addToDisplayGroupCollection (
        ComponentGroupable * groupable,
        ComponentIdentifiable * identifiable,
        std::vector<int> const & source,
        std::vector<DisplayGroup> & destination) const;

    void addToDisplayGroupCollection (
        ComponentGroupable * groupable,
        ComponentIdentifiable * identifiable,
        DisplayGroup const & source,
        std::vector<DisplayGroup> & destination) const;

    void addToDisplayGroupCollection (
        ComponentGroupable * groupable,
        ComponentIdentifiable * identifiable,
        DisplayItem const & source,
        std::vector<DisplayGroup> & destination) const;

    DisplayGroup removeFromDisplayGroupCollection (
        ComponentIdentifiable * identifiable,
        std::vector<DisplayGroup> & source,
        unsigned int index,
        unsigned int count) const;

    std::vector<int> displayGroupCollectionToGameItems (
        ComponentIdentifiable * identifiable,
        std::vector<DisplayGroup> const & source) const;

    void transferItems (
        std::vector<DisplayGroup> & source,
        std::vector<DisplayGroup> & destination,
        unsigned int index,
        unsigned int count) const;

    void handleTransfer (
        Game * game,
        std::string const & promptMessage,
        std::vector<DisplayGroup> & source,
        std::vector<DisplayGroup> & destination) const;

    void initializeResources ();
    void handleTake ();
    void handleDrop ();
    void handleDone ();

    GameItem * mCharacter;
    GameItem * mTile;
    std::vector<DisplayGroup> mItemsOwned;
    std::vector<DisplayGroup> mItemsAvailable;
};

#endif // GAMESTATEINVENTORY_H
