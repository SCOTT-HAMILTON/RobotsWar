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
    virtual ~FunctionBlock() = default;
    bool getCommands(size_t nbtCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands, bool &commands_ended, std::vector<std::string> &argval);


private:
    std::vector<std::string> argsname;
};

#endif // FUNCTIONBLOCK_H
