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
    ElseIfBlock(const std::string &boolexpr, std::weak_ptr<ScriptBlock> precblock);
    virtual ~ElseIfBlock();
    virtual bool canEnter();
    virtual size_t getCommands(size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commands_ended);
    idtype getMyId() const;

private:
    std::weak_ptr<ScriptBlock> prec_block;
};

#endif // ELSEIFBLOCK_H
