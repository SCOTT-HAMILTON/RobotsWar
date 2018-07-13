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
    BlockEntryCommand(std::weak_ptr<ScriptBlock> block, std::size_t nb_cmds, std::shared_ptr<bool> resultOfTest = nullptr);
    virtual ~BlockEntryCommand();
    virtual void update();
    std::weak_ptr<bool> couldEnter;
};

#endif // BLOCKENTRYCOMMAND_H
