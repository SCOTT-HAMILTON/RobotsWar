#include "Commands/ScriptCommand.h"

ScriptCommand::ScriptCommand(ScriptBlock &block, const std::string &_type) :
    myblock(block), type(_type)
{
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

ScriptBlock &ScriptCommand::getBlock(){
    return myblock;
}

void ScriptCommand::setProp(const std::string &name, double val){
    props[name] = val;
}
