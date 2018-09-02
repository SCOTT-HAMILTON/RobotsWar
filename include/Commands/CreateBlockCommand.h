#ifndef CREATEBLOCKCOMMAND_H
#define CREATEBLOCKCOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

class CreateBlockCommand : public ScriptCommand
{
public:
    CreateBlockCommand(ScriptBlock &block, const ParamVar &blockrel, const ParamVar &blockid);
    virtual ~CreateBlockCommand();
    virtual void update();
};

#endif // CREATEBLOCKCOMMAND_H
