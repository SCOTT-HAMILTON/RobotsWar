#ifndef ELSEIFBLOCK_H
#define ELSEIFBLOCK_H

#include <iostream>
#include <string>

#include "IfBlock.h"

class ElseIfBlock : public IfBlock
{
public:
    ElseIfBlock(const std::string &boolexpr, std::weak_ptr<ScriptBlock> precblock);
    virtual ~ElseIfBlock();
    virtual bool canEnter();

private:
    std::weak_ptr<ScriptBlock> prec_block;
};

#endif // ELSEIFBLOCK_H
