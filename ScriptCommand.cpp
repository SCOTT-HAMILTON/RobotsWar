#include "ScriptCommand.h"

ScriptCommand::ScriptCommand(const std::string &type)
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

const vartype &ScriptCommand::getProp(const std::string &name){
    return props[name];
}

const std::string &ScriptCommand::getStringProp(const std::string &name){
    return strings[name];
}
