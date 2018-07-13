#include "ConditionBlock.h"

ConditionBlock::ConditionBlock(const std::string &boolexpr) :
    ScriptBlock("conditionblock"), expr(boolexpr)
{
}


ConditionBlock::~ConditionBlock()
{
    //dtor
}

size_t ConditionBlock::getCommands(size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commands_ended){
    return ScriptBlock::getCommands(static_cast<size_t>(nbCommands), pCommands, commands_ended);
}
