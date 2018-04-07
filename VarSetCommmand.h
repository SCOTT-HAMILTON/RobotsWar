#ifndef VARSETCOMMMAND_H
#define VARSETCOMMMAND_H

#include <iostream>
#include <string>
#include <memory>

#include "ScriptCommand.h"
#include "ScriptBlock.h"

class VarSetCommmand : public ScriptCommand
{
public:
    VarSetCommmand(std::shared_ptr<ScriptBlock> block, const std::string &expr, const std::string &var);
    virtual ~VarSetCommmand();

    virtual void update();

private:
    std::weak_ptr<ScriptBlock> myblock;
    std::string expr;
    std::string varname;
    float value;
};

#endif // VARSETCOMMMAND_H
