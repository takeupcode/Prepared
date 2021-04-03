#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include <string>
#include <vector>

enum class ColorMode
{
    None,
    Basic,
    EightBit,
    True
};

inline std::string ColorModeToString (ColorMode value)
{
    switch (value)
    {
    case ColorMode::None:
        return "No color";

    case ColorMode::Basic:
        return "16 colors";

    case ColorMode::EightBit:
        return "256 colors";

    case ColorMode::True:
        return "True colors";

    default:
        return "Unknown";
    }
}

inline std::vector<std::string> ColorModeToVector ()
{
    std::vector<std::string> values;

    values.push_back(ColorModeToString(ColorMode::None));
    values.push_back(ColorModeToString(ColorMode::Basic));
    values.push_back(ColorModeToString(ColorMode::EightBit));
    values.push_back(ColorModeToString(ColorMode::True));

    return values;
}

class GameOptions
{
public:
    ColorMode & colorMode ()
    {
        return mColorMode;
    }

private:
    ColorMode mColorMode = ColorMode::None;
};

#endif // GAMEOPTIONS_H
