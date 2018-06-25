#include "WhileBlock.h"
#include "BlockEntryCommand.h"

WhileBlock::WhileBlock(const std::string &boolexpr) :
    ConditionBlock(boolexpr)
{
    type = "whileblock";
}

WhileBlock::~WhileBlock()
{
    //dtor
}

bool WhileBlock::canEnter(){
    stat = true;
    if (index_lastcmd == 0){
        double val;
        if (evalParserExpr(expr, val)){
            stat = false;
            condchain_entered = stat;
            return stat;
        }

        stat = static_cast<bool>( val );
        condchain_entered = stat;
        return stat;
    }
    return stat;
}

std::size_t WhileBlock::getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commandsended){
    std::size_t start_size = pCommands.size();
    int i = 0;
    size_t nb_cmd = 0;
    commandsended = false;
    ownedcommands.clear();
    if (canEnter()){
        do{
            auto ptrcmd = std::make_shared<BlockEntryCommand>(me, 0);
            ownedcommands.push_back(ptrcmd);
            if (pCommands.back().lock()->getType() != "blockentry")pCommands.push_back(ownedcommands.back());
            size_t size_before = pCommands.size();
            while (!commandsended && nb_cmd < nbCommands)nb_cmd += ScriptBlock::getCommands(nbCommands-(pCommands.size()-start_size), pCommands, commandsended);
            commandsended = false;
            ptrcmd->setProp("nbcmd", pCommands.size()-size_before);
            i++;
        }while ( nb_cmd < nbCommands);
    }else commandsended = true;

    return nb_cmd;
}

