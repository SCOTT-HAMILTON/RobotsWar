#include "ForBlock.h"

#include "VarSetCommand.h"

idtype ForBlock::curid = 0;

ForBlock::ForBlock(const std::string &var, const std::string &boolexpr, const std::string &startexpr, const std::string &exprincremente) :
    ConditionBlock(boolexpr), incrementer(exprincremente), varname(var), myid(curid), first(true), all_cmds_done(true)
{
    curid++;
    type = "forblock";
    this->startexpr = startexpr;
    double val = 0;
    if (varExist(startexpr)){
        val = getVar(startexpr);
    }else evalParserExpr(startexpr, val);
    addVar(varname, val);
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

bool ForBlock::getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commandsended){
    auto cmd_blockentry = std::shared_ptr<BlockEntryCommand>();
    bool entry_first_already_set = false;
    if (pCommands.size()>0 && pCommands.back().lock()->getType() == "blockentry_forblock"){
        cmd_blockentry = std::static_pointer_cast<BlockEntryCommand>(pCommands.back().lock());
        entry_first_already_set = true;
    }
    size_t nb_cmds_start = pCommands.size();
    if (cmd_last_eval_entry != nullptr && !cmd_last_eval_entry->getProp("canenter")){
        first = true;
        commandsended = true;
        return true;
    }
    if (first){
        pCommands.push_back(commands[0]);//add init varset!!
    }
    first = false;
    bool can = true;

    if (!all_cmds_done){
        all_cmds_done = ScriptBlock::getCommands(nbCommands, pCommands, commandsended);
        if (entry_first_already_set){
            cmd_last_eval_entry = cmd_blockentry;
            //std::cout << "already set (in cleaning)!!!\n";
        }
        if (!all_cmds_done)return false;
        pCommands.push_back(commands[1]);
        if (entry_first_already_set){
            cmd_blockentry->setProp("nbcmd", pCommands.size()-nb_cmds_start);
        }
        entry_first_already_set = false;
        /*
        std::cout << "forblock properly ended cmds!! : \n";
        for (size_t i = 0; i < pCommands.size(); i++){
            if (pCommands[i].lock()->getType() == "varset"){
               std::cout << "cmd post clean forblock : " << pCommands[i].lock()->getStringProp("varname") << " = " << pCommands[i].lock()->getStringProp("expr") << "\n";
            }
            else std::cout << "cmd post clean forblock : " << pCommands[i].lock()->getType() << "\n";
        }*/

    }



    while (can && pCommands.size()<=nbCommands){
        if (entry_first_already_set){
            cmd_last_eval_entry = cmd_blockentry;
            //std::cout << "already set!!!\n";
        }else{
            cmd_blockentry = std::make_shared<BlockEntryCommand>(me, 0);
            cmd_last_eval_entry = cmd_blockentry;
            tempcommands.push_back(cmd_blockentry);
            pCommands.push_back(cmd_blockentry);
        }
        size_t pre_size = pCommands.size();
        all_cmds_done = ScriptBlock::getCommands(nbCommands, pCommands, commandsended);
        if (entry_first_already_set){
            cmd_blockentry->setProp("nbcmd", pCommands.size()-nb_cmds_start);
        }
        else {
            if (pCommands.size()-pre_size == 0)pCommands.erase(pCommands.begin()+pre_size-1);
            else{
                cmd_blockentry->setProp("nbcmd", pCommands.size()-pre_size);
            }
        }
        if (all_cmds_done){
            pCommands.push_back(commands[1]); //add incremente varset

        }else{
            can = false;
        }
        entry_first_already_set = false;
    }
    //std::cout << "\n for block " << myid << "get cmds exit !!\n";
    commandsended = false;
    return false;
}
