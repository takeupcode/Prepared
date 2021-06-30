#ifndef GAMEITEMREGISTRY_H
#define GAMEITEMREGISTRY_H

#include <map>
#include <string>
#include <unordered_map>

class GameItem;

class GameItemRegistry
{
public:
    static GameItem * add (std::string const & permanentId);

    static GameItem * find (int id);
    static GameItem * find (std::string const & permanentId);

    static void clear ();

private:
    using PermanentIdMap =
        std::map<std::string, int>;
    using GameItemMap =
        std::unordered_map<int, std::unique_ptr<GameItem>>;

    static int mNextId;
    static PermanentIdMap mGameItemIds;
    static GameItemMap mGameItems;
};

#endif // GAMEITEMREGISTRY_H
