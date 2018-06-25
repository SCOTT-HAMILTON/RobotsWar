#ifndef DESTROYBLOCKCOMMAND_H
#define DESTROYBLOCKCOMMAND_H

#include <iostream>

#include "ScriptCommand.h"


class DestroyBlockCommand : public ScriptCommand
{
public:
    DestroyBlockCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &blockparam);
    virtual ~DestroyBlockCommand();
    virtual void update();

private:
    RELATIF_BLOCK relblock;
};

#endif // DESTROYBLOCKCOMMAND_H
