#include "ForBlock.h"

ForBlock::ForBlock(const std::string &var, const std::string &boolexpr, const std::string &startexpr, const std::string &exprincremente) :
    ConditionBlock(boolexpr), varname(var), incrementer(exprincremente)
{
    type = "forblock";
    this->startexpr = startexpr;
    double val = 0;
    if (varExist(startexpr)){
        val = getVar(startexpr);
    }else evalParserExpr(startexpr, val);
    std::cout << "varname, val : " << varname << ", val : " << val << std::endl;
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
    }else evalParserExpr(expr, val);
    return val;
}

std::size_t ForBlock::getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commandsended){
    std::size_t start_size = pCommands.size();
    bool can = true;
    int i = 0;
    double val = getValFromExpr(startexpr);
    addVar(varname, val);
    for (double i = val; can && nbCommands-(pCommands.size()-start_size)>0; i+=getValFromExpr(incrementer)){
        ScriptBlock::getCommands(nbCommands-(pCommands.size()-start_size), pCommands, commandsended);
        addVar(varname, i);
        can = canEnter();
    }
    if (can){
        commandsended = false;
    }else {
        commandsended = true;
    }
    val = getValFromExpr(startexpr);
    addVar(varname, val);
}
