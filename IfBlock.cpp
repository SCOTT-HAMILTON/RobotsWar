#include "IfBlock.h"

idtype IfBlock::curid = 0;

IfBlock::IfBlock(const std::string &boolexpr) :
    ConditionBlock(boolexpr), myid(curid), stillCan(false), prec_indexlastcmd(0)
{
    std::cout << "new if block my id : " << myid << std::endl;
    curid++;
    type = "ifblock";
}

IfBlock::~IfBlock()
{
    //dtor
}

bool IfBlock::canEnter(){
    if (stillCan){
        if (prec_indexlastcmd != 0){
            stat = true;
            condchain_entered = true;
            return stat;
        }
    }

    double val;
    if (evalParserExpr(expr, val)){
        stat = false;
        std::cout << "ifblock error evaluating expr " << expr << std::endl;
        condchain_entered = stat;
        return stat;
    }

    stat = static_cast<bool>(val);

    stillCan = stat;

    condchain_entered = stat;
    return stat;
}

size_t IfBlock::getCommands(size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commands_ended){
    prec_indexlastcmd = index_lastcmd;
    return ConditionBlock::getCommands(nbCommands, pCommands, commands_ended);
}
