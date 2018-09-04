#include "Blocks/WhileBlock.h"
#include "Commands/BlockEntryCommand.h"

WhileBlock::WhileBlock(const std::string &boolexpr) :
    ConditionBlock(boolexpr)
{
    type = "whileblock";
    add_blockentry_test_on_getcmds = true;
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

bool WhileBlock::getCommands(std::size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands, bool &commandsended){
    std::size_t start_size = pCommands.size();
    size_t nb_cmd = 0;
    commandsended = false;
    ownedcommands.clear();
    if (index_lastcmd == 0 && last_could_enter == false){
        commandsended = true;
        return 0;
    }
    ScriptCommand *ptrcmdEntry;

    {
        auto tmpcmd = std::make_unique<BlockEntryCommand>(static_cast<BlockEntryCommand&>(commands[0].get()));
        ptrcmdEntry = tmpcmd.get();
        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcmdEntry);
        pCommands.push_back( static_cast<std::reference_wrapper<ScriptCommand>>( ref_cmd ));
        ownedcommands.push_back(std::move(tmpcmd));
    }

    size_t size_before = pCommands.size();
    while (!commandsended && nb_cmd < nbCommands)nb_cmd += ScriptBlock::getCommands(nbCommands-(pCommands.size()-start_size), pCommands, commandsended);
    commandsended = false;
    ptrcmdEntry->setProp("nbcmd", pCommands.size()-size_before);

    auto tmpcmd = std::make_unique<BlockEntryCommand>( static_cast<BlockEntryCommand&>(commands[0].get()) );
    ptrcmdEntry = tmpcmd.get();
    std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcmdEntry);
    pCommands.push_back( static_cast<std::reference_wrapper<ScriptCommand>>( ref_cmd ));
    ownedcommands.push_back(std::move(tmpcmd));


    return nb_cmd;
}

