#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <iostream>
#include <string>
#include <variant>
#include <map>
#include <memory>

class ScriptBlock;

#include "Blocks/ScriptBlock.h"

enum TYPE{VAR, CONSTANT};

enum RELATIF_BLOCK{BLOCKRIGHT, BLOCKLEFT, BLOCKUP, BLOCKDOWN};

struct ParamVar{
    TYPE type;
    std::string expr;
    double doubleval;
};

class ScriptCommand
{
public:
    ScriptCommand();
    ScriptCommand(ScriptBlock &block, const std::string &type);
    virtual ~ScriptCommand();
    const std::string &getType();
    double getProp(const std::string &name);
    const std::string &getStringProp(const std::string &name);
    virtual void update(){}
    void setProp(const std::string &name, double val);
    ScriptBlock &getBlock();

protected:
    ScriptBlock &myblock;
    std::string type;
    std::map<std::string, double> props;
    std::map<std::string, std::string> strings;
};

#endif // SCRIPTCOMMAND_H
