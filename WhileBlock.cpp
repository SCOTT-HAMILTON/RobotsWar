#include "WhileBlock.h"
#include "BlockEntryCommand.h"

WhileBlock::WhileBlock(const std::string &boolexpr) :
    ConditionBlock(boolexpr)
{
    resultLastEntryTest = std::make_shared<bool>(true);
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

bool WhileBlock::getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commandsended){
    std::size_t start_size = pCommands.size();
    size_t nb_cmd = 0;
    commandsended = false;
    ownedcommands.clear();
    if (index_lastcmd == 0 && *resultLastEntryTest == false){
        commandsended = true;
        return 0;
    }
    std::shared_ptr<ScriptCommand> ptrcmdEntry;

    if (pCommands.back().lock()->getType() != "blockentry_whileblock"){
        ptrcmdEntry = std::make_shared<BlockEntryCommand>(me, 0);
        ownedcommands.push_back(ptrcmdEntry);
        pCommands.push_back(ptrcmdEntry);
    }else{
        ptrcmdEntry = pCommands.back().lock();
    }
    size_t size_before = pCommands.size();
    while (!commandsended && nb_cmd < nbCommands)nb_cmd += ScriptBlock::getCommands(nbCommands-(pCommands.size()-start_size), pCommands, commandsended);
    commandsended = false;
    ptrcmdEntry->setProp("nbcmd", pCommands.size()-size_before);

    ptrcmdEntry = std::make_shared<BlockEntryCommand>(me, 0, resultLastEntryTest);
    ownedcommands.push_back(ptrcmdEntry);
    pCommands.push_back(ptrcmdEntry);


    return nb_cmd;
}

