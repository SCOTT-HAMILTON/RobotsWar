#ifndef SHOOTGUIDEDMISSILECOMMAND_H
#define SHOOTGUIDEDMISSILECOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

using idtype = unsigned long long;

class ShootGuidedMissileCommand : public ScriptCommand
{
public:
    ShootGuidedMissileCommand(ScriptBlock &block);
    virtual ~ShootGuidedMissileCommand();

private:
};

#endif // SHOOTGUIDEDMISSILECOMMAND_H
