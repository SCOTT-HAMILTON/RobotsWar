#include "Blocks/ElseBlock.h"

ElseBlock::ElseBlock(ScriptBlock &precblock) :
    ScriptBlock("elseblock"), prec_block(precblock), stillCan(false), prec_indexlastcmd(0)
{
    type = "elseblock";
}

ElseBlock::~ElseBlock()
{
    //dtor
}

bool ElseBlock::canEnter(){
    if (stillCan){
        if (prec_indexlastcmd != 0){
            stat = true;
            condchain_entered = true;
            return stat;
        }
    }
    stat = !prec_block.isCondChainEntered();
    stillCan = stat;
    //std::cerr << "else : " << stat << '\n';
    return stat;
}

bool ElseBlock::getCommands(size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands, bool &commands_ended){
    prec_indexlastcmd = index_lastcmd;
    return ScriptBlock::getCommands(nbCommands, pCommands, commands_ended);
}
