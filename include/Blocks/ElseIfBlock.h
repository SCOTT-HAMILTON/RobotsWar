#ifndef ELSEIFBLOCK_H
#define ELSEIFBLOCK_H

#include <iostream>
#include <string>
#include <variant>

#include "IfBlock.h"

using idtype = unsigned long long;

class ElseIfBlock : public IfBlock
{
public:
    ElseIfBlock(const std::string &boolexpr, ScriptBlock &precblock);
    virtual ~ElseIfBlock();
    virtual bool canEnter();
    virtual bool getCommands(size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands, bool &commands_ended);
    idtype getMyId() const;

private:
    ScriptBlock &prec_block;
};

#endif // ELSEIFBLOCK_H
