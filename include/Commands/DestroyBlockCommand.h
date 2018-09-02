#ifndef DESTROYBLOCKCOMMAND_H
#define DESTROYBLOCKCOMMAND_H

#include <iostream>

#include "ScriptCommand.h"


class DestroyBlockCommand : public ScriptCommand
{
public:
    DestroyBlockCommand(ScriptBlock &block, const ParamVar &blockparam);
    virtual ~DestroyBlockCommand();
    virtual void update();
};

#endif // DESTROYBLOCKCOMMAND_H
