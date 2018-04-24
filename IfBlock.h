#ifndef IFBLOCK_H
#define IFBLOCK_H

#include <iostream>
#include <string>

#include "ConditionBlock.h"

class IfBlock : public ConditionBlock
{
public:
    IfBlock(const std::string &boolexpr);
    virtual ~IfBlock();
    virtual bool canEnter();

};

#endif // IFBLOCK_H
