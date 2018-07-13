#ifndef GETGMISSILEYCOMMAND_H
#define GETGMISSILEYCOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

class getGMissileYCommand : public ScriptCommand
{
public:
    getGMissileYCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &id);
    virtual ~getGMissileYCommand();
    virtual void update();

private:
    ParamVar myid;
};

#endif // GETGMISSILEYCOMMAND_H
