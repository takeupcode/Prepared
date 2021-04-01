#ifndef ASCII_H
#define ASCII_H

#include <string>
#include <vector>

struct ASCIIGraphic
{
    static char const * ResetAll;
    static char const * BoldIntensity;
    static char const * DimIntensity;
    static char const * ColorRGB;
    static char const * Italic;
    static char const * Underline;
    static char const * SlowBlink;
    static char const * Color256;
    static char const * FastBlink;
    static char const * Negative;
    static char const * Hide;
    static char const * Strike;
    static char const * DoubleUnderline;
    static char const * NormalIntensity;
    static char const * NoItalic;
    static char const * NoUnderline;
    static char const * NoBlink;
    static char const * NoNegative;
    static char const * NoHide;
    static char const * NoStrike;
    static char const * ForeBlack;
    static char const * ForeRed;
    static char const * ForeGreen;
    static char const * ForeYellow;
    static char const * ForeBlue;
    static char const * ForeMagenta;
    static char const * ForeCyan;
    static char const * ForeWhite;
    static char const * SetForeColor;
    static char const * ForeDefault;
    static char const * BackBlack;
    static char const * BackRed;
    static char const * BackGreen;
    static char const * BackYellow;
    static char const * BackBlue;
    static char const * BackMagenta;
    static char const * BackCyan;
    static char const * BackWhite;
    static char const * SetBackColor;
    static char const * BackDefault;
    static char const * Overline;
    static char const * NoOverline;
    static char const * ForeGrey;
    static char const * ForeBrightRed;
    static char const * ForeBrightGreen;
    static char const * ForeBrightYellow;
    static char const * ForeBrightBlue;
    static char const * ForeBrightMagenta;
    static char const * ForeBrightCyan;
    static char const * ForeBrightWhite;
    static char const * BackGrey;
    static char const * BackBrightRed;
    static char const * BackBrightGreen;
    static char const * BackBrightYellow;
    static char const * BackBrightBlue;
    static char const * BackBrightMagenta;
    static char const * BackBrightCyan;
    static char const * BackBrightWhite;
};

class ASCIIEscape
{
public:
    static bool enabled ();

    static void setEnabled (bool enabled);

    static std::string graphicSequence (
        std::vector<char const *> const & attrs);

private:
    static bool mEnabled;
};

#endif // ASCII_H
