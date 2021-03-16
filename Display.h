#ifndef DISPLAY_H
#define DISPLAY_H

#include "Location.h"

#include <ostream>
#include <sstream>
#include <string>
#include <vector>

class Game;

class Display
{
public:
    Display (
        Game * game,
        unsigned int screenHeight = 30);

    Game * game () const;

    void clear ();
    void update ();

    void beginStreamingToMap ();
    void endStreamingToMap ();
    void setMapSymbol (
        char symbol,
        Location const & location,
        bool important = false);

    void beginStreamingToDialog ();
    void endStreamingToDialog ();

    std::ostream & mapBuffer ();
    std::ostream & dialogBuffer ();

private:
    Game * mGame;
    unsigned int mScreenHeight;
    std::stringstream mMapBuffer;
    std::stringstream mDialogBuffer;
    std::vector<std::string> mMapOutputLines;
    std::vector<std::string> mDialogOutputLines;
    std::vector<Location> mImportantLocations;
};

#endif // DISPLAY_H
