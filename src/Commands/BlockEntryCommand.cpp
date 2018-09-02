#include "Commands/BlockEntryCommand.h"

size_t BlockEntryCommand::tmp = 0;

BlockEntryCommand::BlockEntryCommand(ScriptBlock &block, std::reference_wrapper<size_t> nbcmd) :
    ScriptCommand(block, "blockentry"), nb_cmd(nbcmd)
{
    type += "_"+myblock.getType();
    props["nbcmd"] = nb_cmd;
    props["canenter"] = 0;
}

BlockEntryCommand::BlockEntryCommand(BlockEntryCommand &entrycmd) :
    ScriptCommand (entrycmd.myblock, "blockentry"), nb_cmd(entrycmd.nb_cmd)
{
    type += "_"+myblock.getType();
    props["nbcmd"] = nb_cmd;
    props["canenter"] = 0;
}

BlockEntryCommand::~BlockEntryCommand()
{
    //dtor
}

void BlockEntryCommand::update(){
   // std::cerr << "BlockEntryCommand evaluating if can enter...\n";
    bool val = myblock.canEnter();
    //std::cerr << "BlockEntryCommand evaluated : " << val << "\n";
    props["canenter"] = val;
    //std::cerr << "copying nb_cmd..\n";
    size_t nb = nb_cmd.get();
    //std::cerr << "entre deux;..\n";
    props["nbcmd"] = static_cast<double>(nb);
    //std::cerr << "copied!!\n";
    myblock.setLastCouldEnter(val);
}

void BlockEntryCommand::setNbCmd(size_t val)
{
    nb_cmd.get() = val;
}

void BlockEntryCommand::bindNbCmd(std::reference_wrapper<size_t> nbcmd)
{

    nb_cmd = nbcmd;
}

