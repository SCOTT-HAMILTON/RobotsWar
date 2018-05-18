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

struct CurrenBlock{
    std::weak_ptr<ScriptBlock> current_block;
    int nb_in_wait;
};

class ScriptBlock
{
public:
    ScriptBlock(const std::string &type = "basicblock", bool ended = false);
    virtual ~ScriptBlock();
    const std::string &getType();
    double getVar(const std::string &name);
    double *getVarPtr(const std::string &name);
    double *getPersonalVarPtr(const std::string &name);
    std::string getVarName(std::size_t index);
    const std::string &getString(const std::string &name);
    virtual std::size_t getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands, bool &commandsended);
    std::weak_ptr<ScriptBlock> getCurBlock();
    std::weak_ptr<ScriptBlock> getLastEndedBlock();
    std::weak_ptr<ScriptBlock> getFunction(const std::string &name);
    bool getStat();
    bool isCondChainEntered();
    int nbCommands();

    void addVar(const std::string &name, double var);
    void addParentVar(const std::string &name, double *var);
    void addString(const std::string &name, const std::string &str);
    void addCommand(const std::weak_ptr<ScriptCommand> &command);
    int evalParserExpr(const std::string &expr, double &val);
    void setEnded();
    void addBlock(const std::string &block);
    void addBlock(std::shared_ptr<ScriptBlock> block);
    void addBlockEnd();

    bool varExist(const std::string &name);
    bool functionExist(const std::string &name);
    bool isEnded();
    bool asLoopBlock();
    bool allBlocksEnded();
    virtual bool canEnter();

    void displayVars();

protected:
    std::string type;
    std::string name;
    std::map<std::string, double> vars;
    std::map<std::string, double*> parentvars;
    std::map<std::string, std::string> strings;
    std::vector<std::weak_ptr<ScriptCommand>> commands;
    std::vector<std::shared_ptr<ScriptCommand>> tempcommands;
    bool ended;
    bool asloopblock;
    std::vector<std::shared_ptr<ScriptBlock>> blocks;
    std::map<std::string, std::shared_ptr<ScriptBlock>> functions;
    std::map<std::string, std::shared_ptr<ScriptBlock>> parentfunctions;

    CurrenBlock current_block;
    std::weak_ptr<ScriptBlock> loopblock;

    std::vector<std::variant<std::weak_ptr<ScriptBlock>, std::weak_ptr<ScriptCommand>>> commandsorder;

    std::map<std::string, std::shared_ptr<Parser>> exprevaluated;

    int debug_times;
    bool stat;
    bool condchain_entered;

    std::size_t index_lastcmd;
};

#endif // SCRIPTBLOCK_H
