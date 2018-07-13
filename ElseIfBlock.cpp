#include "ElseIfBlock.h"

ElseIfBlock::ElseIfBlock(const std::string &boolexpr, std::weak_ptr<ScriptBlock> precblock) :
    IfBlock(boolexpr), prec_block(precblock)
{
    std::cout << "new elseif block id : " << myid << std::endl;
    curid++;
    type = "elseifblock";

}

ElseIfBlock::~ElseIfBlock()
{
    //dtor
}

bool ElseIfBlock::canEnter(){
    auto ptrPrecCondBlock = prec_block.lock();
    if (ptrPrecCondBlock == nullptr){
        stat = false;
        std::cout << "      ERROR block elseif, precedent block pointed is nullptr !!" << std::endl;
        condchain_entered = false;
        return stat;
    }
    if (ptrPrecCondBlock->isCondChainEntered()){
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

size_t ElseIfBlock::getCommands(size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commands_ended){
    prec_indexlastcmd = index_lastcmd;
    return IfBlock::getCommands(nbCommands, pCommands, commands_ended);
}
