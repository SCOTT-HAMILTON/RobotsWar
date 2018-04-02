#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include <iostream>

#include "ScriptCommand.h"

enum TYPE{VAR, CONSTANT};

struct offset{
    TYPE type;
    std::string strval;
    float floatval;
};

class MoveCommand : public ScriptCommand
{
public:
    MoveCommand(offset x, offset y);
    virtual ~MoveCommand();


private:
    offset offsetx;
    offset offsety;
};

#endif // MOVECOMMAND_H
