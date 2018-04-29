#ifndef BLOCKENTRYCOMMAND_H
#define BLOCKENTRYCOMMAND_H

#include <iostream>
#include <string>
#include <memory>

#include "ScriptCommand.h"

class ScriptBlock;
#include "ScriptBlock.h"

class BlockEntryCommand : public ScriptCommand
{
public:
    BlockEntryCommand(std::weak_ptr<ScriptBlock> block, std::size_t nb_cmds);
    virtual ~BlockEntryCommand();
    virtual void update();
};

#endif // BLOCKENTRYCOMMAND_H
