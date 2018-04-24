#include "IfBlock.h"

IfBlock::IfBlock(const std::string &boolexpr) :
    ConditionBlock(boolexpr)
{
    type = "ifblock";
}

IfBlock::~IfBlock()
{
    //dtor
}

bool IfBlock::canEnter(){
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
