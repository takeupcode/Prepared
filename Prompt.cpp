#include "Prompt.h"

#include <iomanip>
#include <limits>
#include <stdexcept>

Prompt::Prompt (
    std::ostream & output,
    std::istream & input)
: mOutput(output), mInput(input)
{ }

bool Prompt::promptYesOrNo (
    std::string const & prompt)
{
    mOutput << prompt << " y/n: ";

    char input;
    while (true)
    {
        mInput >> input;
        mInput.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        if (input == 'y' || input == 'Y')
        {
            return true;
        }
        else if (input == 'n' || input == 'N')
        {
            return false;
        }
        else
        {
            mOutput << "Please enter y or n: ";
        }
    }
}

int Prompt::promptNumber (
    std::string const & prompt,
    int minimum,
    int maximum)
{
    mOutput << prompt;

    int input;
    while (true)
    {
        while (!(mInput >> input))
        {
            mInput.clear();
            mInput.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            mOutput << "Please enter a valid number: ";
        }

        mInput.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        if (input >= minimum && input <= maximum)
        {
            return input;
        }

        mOutput << "Please enter a number between "
            << minimum << " and " << maximum << ": ";
    }
}

char Prompt::promptLetter (
    std::string const & prompt,
    char minimum,
    char maximum,
    bool makeUpperCase)
{
    mOutput << prompt;

    char minimumLower;
    char minimumUpper;
    if (minimum >= 'a' && minimum <= 'z')
    {
        minimumLower = minimum;
        minimumUpper = toupper(minimum);
    }
    else if (minimum >= 'A' && minimum <= 'Z')
    {
        minimumLower = tolower(minimum);
        minimumUpper = minimum;
    }
    else
    {
        throw std::logic_error(
            "minimum must be a valid letter between a-z.");
    }

    char maximumLower;
    char maximumUpper;
    if (maximum >= 'a' && maximum <= 'z')
    {
        maximumLower = maximum;
        maximumUpper = toupper(maximum);
    }
    else if (maximum >= 'A' && maximum <= 'Z')
    {
        maximumLower = tolower(maximum);
        maximumUpper = maximum;
    }
    else
    {
        throw std::logic_error(
            "maximum must be a valid letter between a-z.");
    }

    char input;
    while (true)
    {
        mInput >> input;
        mInput.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        if ((input >= minimumLower && input <= maximumLower) ||
        (input >= minimumUpper && input <= maximumUpper))
        {
            if (makeUpperCase)
            {
                input = toupper(input);
            }

            return input;
        }

        mOutput << "Please enter a letter between "
            << minimumLower << " and " << maximumLower << ": ";
    }
}

std::string Prompt::promptText (
    std::string const & prompt,
    bool makeUpperCase)
{
    mOutput << prompt;

    std::string input;

    std::getline(mInput, input);

    if (!makeUpperCase)
    {
        return input;
    }

    for (char & letter: input)
    {
        if (letter >= 'a' && letter <= 'z')
        {
            letter = toupper(letter);
        }
    }

    return input;
}

int Prompt::promptChoice (
    std::string const & prompt,
    std::vector<std::string> const & choices)
{
    int i = 0;
    for (auto & choice: choices)
    {
        mOutput << std::setw(3) << i++ << ": "
            << choice << std::endl;
    }
    mOutput << prompt;

    unsigned int input;
    while (true)
    {
        while (!(mInput >> input))
        {
            mInput.clear();
            mInput.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            mOutput << "Please enter a valid number: ";
        }

        mInput.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        if (input >= 0 && input < choices.size())
        {
            return input;
        }

        mOutput << "Please enter a number between "
            << 0 << " and " << (choices.size() - 1) << ": ";
    }
}
