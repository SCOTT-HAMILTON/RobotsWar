#ifndef GETGMISSILEXCOMMAND_H
#define GETGMISSILEXCOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

class getGMissileXCommand : public ScriptCommand
{
public:
    getGMissileXCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &id);
    virtual ~getGMissileXCommand();
    virtual void update();

private:
    ParamVar myid;
};

#endif // GETGMISSILEXCOMMAND_H
