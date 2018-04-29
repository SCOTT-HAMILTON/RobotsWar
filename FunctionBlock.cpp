#include "FunctionBlock.h"

FunctionBlock::FunctionBlock(const std::string &name, const std::vector<std::string> &vars) :
    ScriptBlock("functionblock")
{
    this->name = name;

    std::cout << "vars  FUNCTIONBLOCK : " << std::endl;
    for (auto &it : vars){
        this->vars.emplace(it, 0);
    }
}

FunctionBlock::~FunctionBlock()
{
    //dtor
}
