#ifndef SHOOTGUIDEDMISSILE_H
#define SHOOTGUIDEDMISSILE_H

#include <iostream>

#include "ScriptCommand.h"

using idtype = unsigned long long;

class ShootGuidedMissile : public ScriptCommand
{
public:
    ShootGuidedMissile(std::weak_ptr<ScriptBlock> block);
    virtual ~ShootGuidedMissile();

private:
};

#endif // SHOOTGUIDEDMISSILE_H
