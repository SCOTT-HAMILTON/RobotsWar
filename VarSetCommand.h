#ifndef VarSetCommand_H
#define VarSetCommand_H

#include <iostream>
#include <string>
#include <memory>

#include "ScriptCommand.h"

#include "ScriptBlock.h"

class VarSetCommand : public ScriptCommand
{
public:
    VarSetCommand(std::weak_ptr<ScriptBlock> block, const std::string &expr, const std::string &var);
    virtual ~VarSetCommand();

    virtual void update();

private:
    std::string expr;
    std::string varname;
    float value;
};

#endif // VarSetCommand_H
