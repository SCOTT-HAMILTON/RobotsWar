#ifndef CALLFUNCBLOCK_H
#define CALLFUNCBLOCK_H

#include "Blocks/ScriptBlock.h"
#include "Blocks/FunctionBlock.h"
#include <string>
#include <vector>

class CallFuncBlock : public ScriptBlock
{
public:
    CallFuncBlock(std::vector<std::string> expr, FunctionBlock &funcblock);
    ~CallFuncBlock() = default;
    bool getCommands(size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands, bool &commands_ended);

private:
    FunctionBlock &funccalled;
    std::vector<std::string> argsexpr;
};

#endif //CALLFUNCBLOCK_H
