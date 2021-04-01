#include "ASCII.h"

bool ASCIIEscape::mEnabled = true;

auto ASCIIEscapeBegin = "\x1b[";
auto ASCIIEscapeDelimiter = ";";
auto ASCIIEscapeGraphicEnd = "m";

char const * ASCIIGraphic::ResetAll = "0";
char const * ASCIIGraphic::BoldIntensity = "1";
char const * ASCIIGraphic::DimIntensity = "2";
char const * ASCIIGraphic::ColorRGB = "2";
char const * ASCIIGraphic::Italic = "3";
char const * ASCIIGraphic::Underline = "4";
char const * ASCIIGraphic::SlowBlink = "5";
char const * ASCIIGraphic::Color256 = "5";
char const * ASCIIGraphic::FastBlink = "6";
char const * ASCIIGraphic::Negative = "7";
char const * ASCIIGraphic::Hide = "8";
char const * ASCIIGraphic::Strike = "9";
char const * ASCIIGraphic::DoubleUnderline = "21";
char const * ASCIIGraphic::NormalIntensity = "22";
char const * ASCIIGraphic::NoItalic = "23";
char const * ASCIIGraphic::NoUnderline = "24";
char const * ASCIIGraphic::NoBlink = "25";
char const * ASCIIGraphic::NoNegative = "27";
char const * ASCIIGraphic::NoHide = "28";
char const * ASCIIGraphic::NoStrike = "29";
char const * ASCIIGraphic::ForeBlack = "30";
char const * ASCIIGraphic::ForeRed = "31";
char const * ASCIIGraphic::ForeGreen = "32";
char const * ASCIIGraphic::ForeYellow = "33";
char const * ASCIIGraphic::ForeBlue = "34";
char const * ASCIIGraphic::ForeMagenta = "35";
char const * ASCIIGraphic::ForeCyan = "36";
char const * ASCIIGraphic::ForeWhite = "37";
char const * ASCIIGraphic::SetForeColor = "38";
char const * ASCIIGraphic::ForeDefault = "39";
char const * ASCIIGraphic::BackBlack = "40";
char const * ASCIIGraphic::BackRed = "41";
char const * ASCIIGraphic::BackGreen = "42";
char const * ASCIIGraphic::BackYellow = "43";
char const * ASCIIGraphic::BackBlue = "44";
char const * ASCIIGraphic::BackMagenta = "45";
char const * ASCIIGraphic::BackCyan = "46";
char const * ASCIIGraphic::BackWhite = "47";
char const * ASCIIGraphic::SetBackColor = "48";
char const * ASCIIGraphic::BackDefault = "49";
char const * ASCIIGraphic::Overline = "53";
char const * ASCIIGraphic::NoOverline = "55";
char const * ASCIIGraphic::ForeGrey = "90";
char const * ASCIIGraphic::ForeBrightRed = "91";
char const * ASCIIGraphic::ForeBrightGreen = "92";
char const * ASCIIGraphic::ForeBrightYellow = "93";
char const * ASCIIGraphic::ForeBrightBlue = "94";
char const * ASCIIGraphic::ForeBrightMagenta = "95";
char const * ASCIIGraphic::ForeBrightCyan = "96";
char const * ASCIIGraphic::ForeBrightWhite = "97";
char const * ASCIIGraphic::BackGrey = "100";
char const * ASCIIGraphic::BackBrightRed = "101";
char const * ASCIIGraphic::BackBrightGreen = "102";
char const * ASCIIGraphic::BackBrightYellow = "103";
char const * ASCIIGraphic::BackBrightBlue = "104";
char const * ASCIIGraphic::BackBrightMagenta = "105";
char const * ASCIIGraphic::BackBrightCyan = "106";
char const * ASCIIGraphic::BackBrightWhite = "107";

bool ASCIIEscape::enabled ()
{
    return mEnabled;
}

void ASCIIEscape::setEnabled (bool enabled)
{
    mEnabled = enabled;
}

std::string ASCIIEscape::graphicSequence (
    std::vector<char const *> const & attrs)
{
    std::string result;

    if (mEnabled && !attrs.empty())
    {
        result += ASCIIEscapeBegin;
        result += attrs[0];

        for (std::size_t i = 1; i < attrs.size(); ++i)
        {
            result += ASCIIEscapeDelimiter;
            result += attrs[i];
        }

        result += ASCIIEscapeGraphicEnd;
    }

    return result;
}
