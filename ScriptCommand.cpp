#include "ScriptCommand.h"

ScriptCommand::ScriptCommand(std::weak_ptr<ScriptBlock> block, const std::string &type) :
    myblock(block)
{
    this->type = type;
}

ScriptCommand::~ScriptCommand()
{
    //dtor
}

const std::string &ScriptCommand::getType(){
    return type;
}

double ScriptCommand::getProp(const std::string &name){
    return props[name];
}

const std::string &ScriptCommand::getStringProp(const std::string &name){
    return strings[name];
}

void ScriptCommand::setBlock(std::weak_ptr<ScriptBlock> block){
    myblock = block;
}

void ScriptCommand::setProp(const std::string &name, double val){
    props[name] = val;
}
