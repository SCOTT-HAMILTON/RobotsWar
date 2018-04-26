#ifndef FORBLOCK_H
#define FORBLOCK_H

#include <iostream>

#include "ConditionBlock.h"

class ForBlock : public ConditionBlock
{
public:
    ForBlock(const std::string &var, const std::string &boolexpr, const std::string &startexpr, const std::string &exprincremente);
    virtual ~ForBlock();
    virtual bool canEnter();
    virtual std::size_t getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands, bool &commandsended);

private:
    std::string incrementer;
    std::string varname;
    std::string startexpr;
    double getValFromExpr(const std::string &expr);
};

#endif // FORBLOCK_H
