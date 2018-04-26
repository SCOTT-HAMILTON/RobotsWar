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
    virtual std::size_t getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands, bool &commandsended);
};

#endif // WHILEBLOCK_H
