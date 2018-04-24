#ifndef ELSE_H
#define ELSE_H

#include <iostream>
#include <string>

#include "ScriptBlock.h"

class ElseBlock : public ScriptBlock
{
public:
    ElseBlock(std::weak_ptr<ScriptBlock> precblock);
    virtual ~ElseBlock();
    virtual bool canEnter();

private:
    std::weak_ptr<ScriptBlock> prec_block;
};

#endif // ELSE_H
