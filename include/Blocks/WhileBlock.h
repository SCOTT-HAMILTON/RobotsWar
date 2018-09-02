#ifndef WHILEBLOCK_H
#define WHILEBLOCK_H

#include <iostream>

#include "ConditionBlock.h"

class WhileBlock : public ConditionBlock
{
public:
    WhileBlock(const std::string &boolexpr);
    virtual ~WhileBlock();
    virtual bool canEnter();
    virtual bool getCommands(std::size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &commands, bool &commandsended);

private:
    std::vector<std::unique_ptr<ScriptCommand>> ownedcommands;

};

#endif // WHILEBLOCK_H
