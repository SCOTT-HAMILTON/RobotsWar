#include "Blocks/FunctionBlock.h"
#include "Commands/VarSetCommand.h"

FunctionBlock::FunctionBlock(const std::string &name, const std::vector<std::string> &vars) :
    ScriptBlock("functionblock"), argsname(vars)
{
    this->name = name;

    std::cerr << "vars  FUNCTIONBLOCK : \n";
    for (size_t i = 0; i < vars.size(); i++){
        addVar(vars[i], 0);
        std::cerr << "added var \"" << vars[i] << "\"\n";
    }
}

bool FunctionBlock::getCommands(size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand> > &pCommands, bool &commands_ended, std::vector<std::string> &argval)
{
    if (argval.size() != argsname.size())std::cerr << "erreur func " << name << ", getcmds, invalid number of args dropped!!\n";
    else{
        for (size_t i = 0; i < argsname.size(); i++){
            auto cmd = std::make_unique<VarSetCommand>(static_cast<VarSetCommand&>(commands[0].get()));
            cmd->setVarname(argsname[i]);
            cmd->setExpr(argval[i]);
            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*static_cast<ScriptCommand*>(cmd.get()));
            pCommands.push_back(ref_cmd);
            tempcommands.push_back(std::move(cmd));
        }
    }

    return ScriptBlock::getCommands(nbCommands, pCommands, commands_ended);
}

