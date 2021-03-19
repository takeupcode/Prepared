#ifndef GAMEITEMREGISTRY_H
#define GAMEITEMREGISTRY_H

#include <memory>
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
    static int mNextId;
    static std::unordered_map<std::string, int> mGameItemIds;
    static std::unordered_map<int, std::unique_ptr<GameItem>> mGameItems;
};

#endif // GAMEITEMREGISTRY_H
