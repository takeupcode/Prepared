#ifndef DISPLAY_H
#define DISPLAY_H

#include "Point.h"

#include <sstream>
#include <string>
#include <vector>

class Game;

class Display
{
public:
    static int constexpr MapDisplayWidth = 15;
    static int constexpr MapDisplayHeight = 15;

    Display (
        Game * game,
        unsigned int screenHeight = 40);

    Game * game () const;

    void clear ();
    void update ();

    void beginStreamingToMap ();
    void endStreamingToMap ();
    void setMapSymbol (
        char symbol,
        Point const & location,
        bool important = false);

    Point mapScrollOrigin ();
    void ensureVisibleInMap (
        Point const & location,
        int mapWidth,
        int mapHeight);

    void beginStreamingToDialog ();
    void endStreamingToDialog ();

    std::ostream & mapBuffer ();
    std::ostream & dialogBuffer ();

private:
    static int constexpr ScrollingMargin = 4;

    Game * mGame;
    unsigned int mScreenHeight;
    std::stringstream mMapBuffer;
    std::stringstream mDialogBuffer;
    std::vector<std::string> mMapOutputLines;
    std::vector<std::string> mDialogOutputLines;
    std::vector<Point> mImportantLocations;
    Point mMapScrollOrigin;
};

#endif // DISPLAY_H
