#ifndef SHOOTMISSILECOMMAND_H
#define SHOOTMISSILECOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

class ShootMissileCommand : public ScriptCommand
{
public:
    ShootMissileCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &x);
    virtual ~ShootMissileCommand();
    virtual void update();

private:
    ParamVar offsetx;
};

#endif // SHOOTMISSILECOMMAND_H
