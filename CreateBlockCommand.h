#ifndef CREATEBLOCKCOMMAND_H
#define CREATEBLOCKCOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

class CreateBlockCommand : public ScriptCommand
{
public:
    CreateBlockCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &blockrel, const ParamVar &blockid);
    virtual ~CreateBlockCommand();
    virtual void update();

private:
    RELATIF_BLOCK relblock;
    int id;
};

#endif // CREATEBLOCKCOMMAND_H
