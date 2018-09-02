#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

class MoveCommand : public ScriptCommand
{
public:
    MoveCommand(ScriptBlock &block, const ParamVar &x, const ParamVar &y);
    virtual ~MoveCommand();
    virtual void update();

private:
    ParamVar offsetx;
    ParamVar offsety;
};

#endif // MOVECOMMAND_H
