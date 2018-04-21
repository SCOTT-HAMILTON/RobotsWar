#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

enum TYPE{VAR, CONSTANT};

struct offset{
    TYPE type;
    std::string strval;
    double doubleval;
};

class MoveCommand : public ScriptCommand
{
public:
    MoveCommand(std::shared_ptr<ScriptBlock> block, offset x, offset y);
    virtual ~MoveCommand();
    virtual void update();

private:
    offset offsetx;
    offset offsety;
};

#endif // MOVECOMMAND_H
