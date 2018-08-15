#include "FunctionBlock.h"

FunctionBlock::FunctionBlock(const std::string &name, const std::vector<std::string> &vars) :
    ScriptBlock("functionblock")
{
    this->name = name;

    std::cout << "vars  FUNCTIONBLOCK : \n";
    for (size_t i = 0; i < vars.size(); i++){
        addVar(vars[i], 0);
        std::cout << "added var \"" << vars[i] << "\"\n";
    }
}

FunctionBlock::~FunctionBlock()
{
    //dtor
}
