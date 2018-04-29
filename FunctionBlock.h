#ifndef FUNCTIONBLOCK_H
#define FUNCTIONBLOCK_H

#include <iostream>
#include <string>
#include <vector>

#include "ScriptBlock.h"

class FunctionBlock : public ScriptBlock
{
public:
    FunctionBlock(const std::string &name, const std::vector<std::string> &vars);
    virtual ~FunctionBlock();

private:
};

#endif // FUNCTIONBLOCK_H
