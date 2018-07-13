#include "ElseBlock.h"

ElseBlock::ElseBlock(std::weak_ptr<ScriptBlock> precblock) :
    ScriptBlock(), prec_block(precblock), stillCan(false), prec_indexlastcmd(0)
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

    auto ptr = prec_block.lock();
    if (ptr == nullptr){
        stat = false;
        std::cout << "      ERROR block else, block pointed is nullptr !!" << std::endl;
        return stat;
    }
    stat = !ptr->isCondChainEntered();
    stillCan = stat;
    //std::cout << "else : " << stat << std::endl;
    return stat;
}

size_t ElseBlock::getCommands(size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commands_ended){
    prec_indexlastcmd = index_lastcmd;
    return ScriptBlock::getCommands(nbCommands, pCommands, commands_ended);
}
