#ifndef PROMPT_H
#define PROMPT_H

#include <istream>
#include <ostream>
#include <string>
#include <vector>

class Prompt
{
public:
    Prompt (
        std::ostream & output,
        std::istream & input);

    bool promptYesOrNo (
        std::string const & prompt);

    int promptNumber (
        std::string const & prompt,
        int minimum = 0,
        int maximum = 9);

    char promptLetter (
        std::string const & prompt,
        char minimum = 'a',
        char maximum = 'z',
        bool makeUpperCase = false);

    std::string promptText (
        std::string const & prompt,
        bool makeUpperCase = false);

    int promptChoice (
        std::string const & prompt,
        std::vector<std::string> const & choices);

private:
    std::ostream & mOutput;
    std::istream & mInput;
};

#endif // PROMPT_H
