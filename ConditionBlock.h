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

protected:
    std::string expr;
};

#endif // CONDITIONBLOCK_H
