#include "Blocks/CallFuncBlock.h"
#include "Commands/VarSetCommand.h"

CallFuncBlock::CallFuncBlock(std::vector<std::string> expr, FunctionBlock &funcblock) :
    ScriptBlock("callblock"), funccalled(funcblock), argsexpr(expr)
{
}

bool CallFuncBlock::getCommands(size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand> > &pCommands, bool &commands_ended){
    parentvars.emplace("funcreturn", funccalled.getVarPtr("return"));
    bool commandsallended = funccalled.getCommands(nbCommands, pCommands, commands_ended, argsexpr);

    if (commandsallended){
        auto cmd = std::make_unique<VarSetCommand>(static_cast<VarSetCommand&>(commands[0].get()));
        cmd->setExpr("funcreturn");
        cmd->setVarname("return");
        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*static_cast<ScriptCommand*>(cmd.get()));
        pCommands.push_back(ref_cmd);
        tempcommands.push_back(std::move(cmd));
    }
    return commandsallended;
}


