#include "WhileBlock.h"

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
    bool can = true;
    int i = 0;
    do{
        ScriptBlock::getCommands(nbCommands-(pCommands.size()-start_size), pCommands, commandsended);
        can = canEnter();
        i ++;
    }while (can && nbCommands-(pCommands.size()-start_size)>0);
    if (can){
        commandsended = false;
    }else {
        commandsended = true;
     }
}

