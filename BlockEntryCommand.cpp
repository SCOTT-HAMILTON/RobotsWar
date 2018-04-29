#include "BlockEntryCommand.h"

BlockEntryCommand::BlockEntryCommand(std::weak_ptr<ScriptBlock> block, std::size_t nb_cmds) :
    ScriptCommand(block, "blockentry")
{
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
        props["canenter"] = ptr->canEnter();
    }else std::cout << "Error update block entry cmd, block is nullptr!!" << std::endl;
}

