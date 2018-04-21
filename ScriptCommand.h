#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <iostream>
#include <string>
#include <variant>
#include <map>
#include <memory>

class ScriptBlock;

#include "ScriptBlock.h"

class ScriptCommand
{
public:
    ScriptCommand(std::weak_ptr<ScriptBlock> block, const std::string &type);
    virtual ~ScriptCommand();
    const std::string &getType();
    double getProp(const std::string &name);
    const std::string &getStringProp(const std::string &name);
    virtual void update(){}

protected:
    std::weak_ptr<ScriptBlock> myblock;
    std::string type;
    std::map<std::string, double> props;
    std::map<std::string, std::string> strings;
};

#endif // SCRIPTCOMMAND_H
