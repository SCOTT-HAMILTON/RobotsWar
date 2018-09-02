#ifndef BLOCKENTRYCOMMAND_H
#define BLOCKENTRYCOMMAND_H

#include <iostream>
#include <string>
#include <memory>

#include "ScriptCommand.h"

class ScriptBlock;
#include "Blocks/ScriptBlock.h"

class BlockEntryCommand : public ScriptCommand
{
public:
    BlockEntryCommand(ScriptBlock &block, std::reference_wrapper<size_t> nbcmd = tmp);
    BlockEntryCommand(BlockEntryCommand &entrycmd);
    virtual ~BlockEntryCommand();
    virtual void update();
    void bindNbCmd(std::reference_wrapper<size_t> nbcmd);
    void setNbCmd(size_t val);

private:
    static size_t tmp;
    std::reference_wrapper<size_t>nb_cmd;
};

#endif // BLOCKENTRYCOMMAND_H
