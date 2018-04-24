#include "ConditionBlock.h"

ConditionBlock::ConditionBlock(const std::string &boolexpr) :
    ScriptBlock("conditionblock"), expr(boolexpr)
{
    canEnter();
}


ConditionBlock::~ConditionBlock()
{
    //dtor
}
