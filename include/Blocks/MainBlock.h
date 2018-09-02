#ifndef MAINBLOCK_H
#define MAINBLOCK_H

#include "Blocks/ScriptBlock.h"

class MainBlock : public ScriptBlock
{
public:
    MainBlock();
    virtual ~MainBlock();
    bool getCommands(size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands, bool &commands_ended) override;
    void addCommand(std::reference_wrapper<ScriptCommand> command) override;
    void addBlock(const std::string &block) override;
    void addBlock(std::reference_wrapper<ScriptBlock> block) override;
    void addFunctionBlock(std::reference_wrapper<ScriptBlock> funcblock);
};

#endif //MAINBLOCK_H
