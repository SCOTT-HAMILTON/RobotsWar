#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <iostream>
#include <string>
#include <variant>
#include <map>
#include <memory>

class ScriptBlock;

#include "ScriptBlock.h"

enum TYPE{VAR, CONSTANT};

struct ParamVar{
    TYPE type;
    std::string expr;
    double doubleval;
};

class ScriptCommand
{
public:
    ScriptCommand(std::weak_ptr<ScriptBlock> block, const std::string &type);
    virtual ~ScriptCommand();
    const std::string &getType();
    double getProp(const std::string &name);
    const std::string &getStringProp(const std::string &name);
    virtual void update(){}
    void setProp(const std::string &name, double val);
    void setBlock(std::weak_ptr<ScriptBlock> block);
    const std::weak_ptr<ScriptBlock> &getBlock();

protected:
    std::weak_ptr<ScriptBlock> myblock;
    std::string type;
    std::map<std::string, double> props;
    std::map<std::string, std::string> strings;
};

#endif // SCRIPTCOMMAND_H
