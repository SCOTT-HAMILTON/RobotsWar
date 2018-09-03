#include "Blocks/ForBlock.h"

#include "Commands/VarSetCommand.h"

//idtype ForBlock::curid = 0;

ForBlock::ForBlock(const std::string &var, const std::string &boolexpr, const std::string &_startexpr, const std::string &exprincremente) :
    ConditionBlock(boolexpr), incrementer(exprincremente), varname(var),
    startexpr(_startexpr), /*myid(curid),*/ all_cmds_done(true), first(true), nb_cmds_dropped(0)
{
    //curid++;
    type = "forblock";
    double val = 0;
    if (varExist(startexpr)){
        val = getVar(startexpr);
    }else evalParserExpr(startexpr, val);
    addVar(varname, val);

    add_blockentry_test_on_getcmds = true;
}

ForBlock::~ForBlock()
{
    //dtor
}

bool ForBlock::canEnter(){
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
        //std::cerr << "expr " << expr << " , stat : " << stat << '\n';
        return stat;
    }
    return stat;
}

double ForBlock::getValFromExpr(const std::string &expr){
    double val = 0;
    if (varExist(expr)){

        val = getVar(expr);
    }else {
        evalParserExpr(expr, val);
    }
    return val;
}

bool ForBlock::getCommands(std::size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands, bool &commandsended){
    BlockEntryCommand *cmd_blockentry = nullptr;
    if (!last_could_enter){
        first = true;
        commandsended = true;
        /*for (auto &c : pCommands){
            auto &cmd = c.get();
        }*/
        last_could_enter = true;
        //std::cerr << "last eval negative for block " << varname << ", can't enter anymore\n";
        return true;
    }
    if (first){
        pCommands.push_back(commands[0]);//add init varset!!
    }
    first = false;
    bool can = true;

    bool first_blockentrytest_added = false;
    size_t start_size = 0;
    if (!all_cmds_done){
        all_cmds_done = ScriptBlock::getCommands(nbCommands, pCommands, commandsended);
        if (!all_cmds_done)return false;
        pCommands.push_back(commands[1]);
    }



    while (can && pCommands.size()<=nbCommands){
        //ADDING BLOCK ENTRY TEST
        std::unique_ptr<ScriptCommand> tmp = std::make_unique<BlockEntryCommand>(static_cast<BlockEntryCommand&>(commands[2].get()) );
        cmd_blockentry =  static_cast<BlockEntryCommand*>(tmp.get());
        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*tmp);
        pCommands.push_back(static_cast<std::reference_wrapper<ScriptCommand>>(ref_cmd));
        tempcommands.push_back( std::move( tmp) );
        if (!first_blockentrytest_added){
            start_size = pCommands.size();
            first_blockentrytest_added = true;
        }

        //DROPPING COMMANDS....
        size_t pre_size = pCommands.size();
        all_cmds_done = ScriptBlock::getCommands(nbCommands, pCommands, commandsended);

        //removing block entry test and empty cmds
        if (pCommands.size()-pre_size == 0)pCommands.erase(pCommands.begin()+static_cast<int>(pre_size)-1);
        else{
            cmd_blockentry->bindNbCmd(std::ref(nb_cmds_dropped));
        }
        if (all_cmds_done){
            pCommands.push_back(commands[1]); //add incremente varset

        }else{
            can = false;
        }
    }

    nb_cmds_dropped = pCommands.size()-start_size;
    commandsended = false;
    return false;
}
