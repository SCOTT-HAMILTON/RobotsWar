#include "Blocks/ElseIfBlock.h"

ElseIfBlock::ElseIfBlock(const std::string &boolexpr, ScriptBlock &precblock) :
    IfBlock(boolexpr), prec_block(precblock)
{
    std::cerr << "new elseif block id : " << myid << '\n';
    curid++;
    type = "elseifblock";

}

ElseIfBlock::~ElseIfBlock()
{
    //dtor
}

bool ElseIfBlock::canEnter(){
    if (prec_block.isCondChainEntered()){
        stat = false;
        condchain_entered = true;
        stillCan = false;//useless
        return false;
    }

    stat = IfBlock::canEnter();

    return stat;
}

idtype ElseIfBlock::getMyId() const{
    return myid;
}

bool ElseIfBlock::getCommands(size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands, bool &commands_ended){
    prec_indexlastcmd = index_lastcmd;
    return IfBlock::getCommands(nbCommands, pCommands, commands_ended);
}
