#ifndef SCRIPTBLOCK_H
#define SCRIPTBLOCK_H

#include <iostream>
#include <string>
#include <variant>
#include <map>
#include <vector>
#include <memory>
#include <fparser.hh>

class ScriptCommand;

#include "ScriptCommand.h"

typedef FunctionParser Parser;

class ScriptBlock;

struct CurrenBlock{
    std::weak_ptr<ScriptBlock> current_block;
    std::size_t index;
    int nb_in_wait;
};

class ScriptBlock
{
public:
    ScriptBlock(const std::string &type = "basicblock", bool ended = false, std::map<std::string, double*> parentvars = std::map<std::string, double*>());
    virtual ~ScriptBlock();
    const std::string &getType();
    double getVar(const std::string &name);
    const std::string &getString(const std::string &name);
    void getCommands(std::size_t start, std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands);
    std::weak_ptr<ScriptBlock> getCurBlock();
    std::weak_ptr<Parser> getParser();
    int nbCommands();

    void addVar(const std::string &name, double var);
    void addString(const std::string &name, const std::string &str);
    void addCommand(const std::weak_ptr<ScriptCommand> &command);
    int evalParserExpr(const std::string &expr, double &val);
    void setEnded();
    void addBlock(const std::string &block);
    void addBlockEnd();

    bool varExist(const std::string &name);
    bool isEnded();
    bool asLoopBlock();
    bool allBlocksEnded();
    virtual bool canEnter();

    void displayVars();

protected:
    std::string type;
    std::map<std::string, double> vars;
    std::map<std::string, double*> parentvars;
    std::map<std::string, std::string> strings;
    std::vector<std::weak_ptr<ScriptCommand>> commands;
    bool ended;
    bool asloopblock;
    std::vector<std::shared_ptr<ScriptBlock>> blocks;
    CurrenBlock current_block;
    std::shared_ptr<Parser> parser;
    std::weak_ptr<ScriptBlock> loopblock;

    int debug_times;
};

#endif // SCRIPTBLOCK_H