#ifndef BEHAVIORCREATURE_H
#define BEHAVIORCREATURE_H

#include "BehaviorNPC.h"

class BehaviorCreature : public BehaviorNPC
{
public:
    void onUpdate (
        Game * game,
        Character * character) override;
};

#endif // BEHAVIORCREATURE_H
