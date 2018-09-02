#ifndef GMISSILETURNCOMMAND_H
#define GMISSILETURNCOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

class GMissileTurnCommand : public ScriptCommand
{
public:
    GMissileTurnCommand(ScriptBlock &block, const ParamVar &id, const ParamVar &angle);
    virtual ~GMissileTurnCommand();
    virtual void update();

private:
    ParamVar myid;
    ParamVar myangle;
};

#endif // GMISSILETURNCOMMAND_H
