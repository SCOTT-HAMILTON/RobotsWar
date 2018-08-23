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
    virtual bool getCommands(size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commands_ended);

private:
    std::weak_ptr<ScriptBlock> prec_block;
    bool stillCan;
    size_t prec_indexlastcmd;
};

#endif // ELSE_H
