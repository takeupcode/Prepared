#ifndef OVERLOADED_H
#define OVERLOADED_H

// Define a helper type and deduction guide for calls to visit.
// The deduction guide should not be needed with C++20 or later.
template<typename... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};
template<typename... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

#endif // OVERLOADED_H
