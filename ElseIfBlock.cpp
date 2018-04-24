#include "ElseIfBlock.h"

ElseIfBlock::ElseIfBlock(const std::string &boolexpr, std::weak_ptr<ScriptBlock> precblock) :
    IfBlock(boolexpr), prec_block(precblock)
{
    type = "elseifblock";
}

ElseIfBlock::~ElseIfBlock()
{
    //dtor
}

bool ElseIfBlock::canEnter(){
    auto ptr = prec_block.lock();
    if (ptr == nullptr){
        stat = false;
        std::cout << "      ERROR block elseif, block pointed is nullptr !!" << std::endl;
        return stat;
    }
    //std::cout << "block type : " << ptr->getStat() << std::endl;
    if (ptr->isCondChainEntered()){
        stat = false;
        condchain_entered = true;
        return false;
    }

    stat = IfBlock::canEnter();
    condchain_entered = stat;
    return stat;
}
