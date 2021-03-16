#ifndef BEHAVIORPC_H
#define BEHAVIORPC_H

#include "Behavior.h"

class BehaviorPC : public Behavior
{
public:
    void onUpdate (
        Game * game,
        Character * character) override;
};

#endif // BEHAVIORPC_H
