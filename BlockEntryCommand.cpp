#include "BlockEntryCommand.h"

BlockEntryCommand::BlockEntryCommand(std::weak_ptr<ScriptBlock> block, std::size_t nb_cmds, std::shared_ptr<bool> resultOfTest) :
    ScriptCommand(block, "blockentry"), couldEnter(resultOfTest)
{
    type += "_"+block.lock()->getType();
    props["nbcmd"] = nb_cmds;
    props["canenter"] = 0;
}

BlockEntryCommand::~BlockEntryCommand()
{
    //dtor
}

void BlockEntryCommand::update(){
    auto ptr = myblock.lock();
    if (ptr != nullptr){
        bool val = ptr->canEnter();
        props["canenter"] = val;
        auto ptrResult = couldEnter.lock();
        if (ptrResult != nullptr){
            *ptrResult = val;
        }
    }else std::cout << "Error update block entry cmd, block is nullptr!!\n";
}

