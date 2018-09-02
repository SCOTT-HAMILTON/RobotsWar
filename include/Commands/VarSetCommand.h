#ifndef VarSetCommand_H
#define VarSetCommand_H

#include <iostream>
#include <string>
#include <memory>

#include "ScriptCommand.h"

#include "Blocks/ScriptBlock.h"

class VarSetCommand : public ScriptCommand
{
public:
    VarSetCommand(ScriptBlock &block, const std::string &expr, const std::string &var);
    VarSetCommand(VarSetCommand &varset);
    virtual ~VarSetCommand();
    void setVarname(const std::string &varname);
    void setExpr(const std::string &expr);

    virtual void update();

private:
    std::string expr;
    std::string varname;
};

#endif // VarSetCommand_H
