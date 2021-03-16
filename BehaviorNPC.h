#ifndef BEHAVIORNPC_H
#define BEHAVIORNPC_H

#include "Behavior.h"

class BehaviorNPC : public Behavior
{
public:
    void onUpdate (
        Game * game,
        Character * character) override;
};

#endif // BEHAVIORNPC_H
