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
    virtual bool getCommands(size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands, bool &commandsended);
    std::weak_ptr<ScriptBlock> getCurBlock();
    std::weak_ptr<ScriptBlock> getLastEndedBlock();
    std::weak_ptr<ScriptBlock> getFunction(const std::string &name);
    bool getStat();
    bool isCondChainEntered();
    int nbCommands();
    bool getResultLastCanEnter();

    void addVar(const std::string &name, double var);
    void addParentVar(const std::string &name, double *var);
    void addString(const std::string &name, const std::string &str);
    void addCommand(const std::weak_ptr<ScriptCommand> &command);
    void addNotPlayedCommand(const std::weak_ptr<ScriptCommand> &command);
    int evalParserExpr(const std::string &expr, double &val);
    void setEnded();
    void addBlock(const std::string &block);
    void addBlock(std::shared_ptr<ScriptBlock> block);
    void addBlockEnd();
    void addFunctionBlock(const std::shared_ptr<ScriptBlock> &funcblock);

    void copyVarsTo(std::weak_ptr<ScriptBlock> block);
    void copyBlocksTo(std::weak_ptr<ScriptBlock> block);
    void copyCommandsOrderTo(std::weak_ptr<ScriptBlock> block);

    bool varExist(const std::string &name);
    bool functionExist(const std::string &name);
    bool isEnded();
    bool asLoopBlock();
    bool allBlocksEnded();
    virtual bool canEnter();

    void displayVars();

    std::weak_ptr<ScriptBlock> me;

protected:
    std::string type;
    std::string name;
    std::map<std::string, double> vars;
    std::map<std::string, double*> parentvars;
    std::map<std::string, std::string> strings;
    std::vector<std::weak_ptr<ScriptCommand>> commands;
    std::vector<std::shared_ptr<ScriptCommand>> tempcommands;
    std::vector<std::shared_ptr<ScriptCommand>> ownedcommands;
    std::vector<std::variant<std::shared_ptr<ScriptBlock>, std::shared_ptr<ScriptCommand>>> ownedruntimecmds;
    bool ended;
    bool asloopblock;
    std::vector<std::shared_ptr<ScriptBlock>> blocks;
    std::map<std::string, std::weak_ptr<ScriptBlock>> functions;
    std::map<std::string, std::weak_ptr<ScriptBlock>> parentfunctions;

    CurrenBlock current_block;
    std::weak_ptr<ScriptBlock> loopblock;

    std::vector<std::variant<std::weak_ptr<ScriptBlock>, std::weak_ptr<ScriptCommand>>> commandsorder;
    std::vector<std::variant<std::weak_ptr<ScriptBlock>, std::weak_ptr<ScriptCommand>>> initOutLoopCommandsOrder;

    std::map<std::string, std::shared_ptr<Parser>> exprevaluated;

    int debug_times;
    bool stat;
    bool condchain_entered;

    std::size_t index_lastcmd;

    bool execInitCommands;
    bool resultLastCanEnter;

};

#endif // SCRIPTBLOCK_H
