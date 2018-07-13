#ifndef CONDITIONBLOCK_H
#define CONDITIONBLOCK_H

#include <iostream>
#include <string>

#include "ScriptBlock.h"

class ConditionBlock : public ScriptBlock
{
public:
    ConditionBlock(const std::string &boolexpr);
    virtual ~ConditionBlock();
    virtual size_t getCommands(size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commands_ended);

protected:
    std::string expr;
};

#endif // CONDITIONBLOCK_H
