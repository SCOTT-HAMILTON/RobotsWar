#include "ElseBlock.h"

ElseBlock::ElseBlock(std::weak_ptr<ScriptBlock> precblock) :
    ScriptBlock(), prec_block(precblock)
{
    type = "elseblock";
}

ElseBlock::~ElseBlock()
{
    //dtor
}

bool ElseBlock::canEnter(){
    auto ptr = prec_block.lock();
    if (ptr == nullptr){
        stat = false;
        std::cout << "      ERROR block else, block pointed is nullptr !!" << std::endl;
        return stat;
    }
    stat = !ptr->isCondChainEntered();

    //std::cout << "else : " << stat << std::endl;
    return stat;
}
