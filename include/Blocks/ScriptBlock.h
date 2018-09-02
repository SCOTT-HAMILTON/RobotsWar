#ifndef SCRIPTBLOCK_H
#define SCRIPTBLOCK_H

#include <iostream>
#include <string>
#include <variant>
#include <map>
#include <vector>
#include <memory>
#include <fparser.hh>
#include <functional>

class ScriptCommand;

#include "Commands/ScriptCommand.h"

typedef FunctionParser Parser;

struct CurrenBlock{
    int nb_in_wait;
    ScriptBlock *current_block;
};

class ScriptBlock
{
public:
    ScriptBlock(const std::string &type = "basicblock", bool ended = false);
    ScriptBlock(ScriptBlock &block) = delete;
    virtual ~ScriptBlock();
    const std::string &getType();
    double getVar(const std::string &name);
    double *getVarPtr(const std::string &name);
    double *getPersonalVarPtr(const std::string &name);
    std::string getVarName(std::size_t index);
    const std::string &getString(const std::string &name);
    virtual bool getCommands(size_t nbCommands, std::vector< std::reference_wrapper<ScriptCommand> > &commands, bool &commandsended);
    ScriptBlock *getCurBlock();
    std::reference_wrapper<ScriptBlock> getLastEndedBlock();
    std::reference_wrapper<ScriptBlock> getFunction(const std::string &name);
    bool getStat();
    bool isCondChainEntered();
    //int nbCommands();
    bool getResultLastCanEnter();
    bool isAddingBlockentryTestOnGetcmds();

    void addMeParentFunction(const std::string &funcname, std::reference_wrapper<ScriptBlock> func);
    void addMeVar(const std::string &varname, double val);
    void addMeParentVar(const std::string &varname, double *val);
    void addVar(const std::string &name, double var);
    void addParentVar(const std::string &name, double *var);
    void addString(const std::string &name, const std::string &str);
    virtual void addCommand(std::reference_wrapper<ScriptCommand> command);
    void addNotPlayedCommand(std::reference_wrapper<ScriptCommand> command);
    int evalParserExpr(const std::string &expr, double &val);
    void setEnded();
    void setLastCouldEnter(bool couldenter);
    virtual void addBlock(const std::string &block);
    virtual void addBlock(std::reference_wrapper<ScriptBlock> block);
    void addBlockEnd();
    size_t nbParentVars();

    void copyVarsTo(ScriptBlock &block);
    void copyBlocksTo(ScriptBlock &block);
    void copyCommandsOrderTo(ScriptBlock &block);

    bool varExist(const std::string &name);
    bool functionExist(const std::string &name);
    bool isEnded();
    bool asLoopBlock();
    bool allBlocksEnded();
    virtual bool canEnter();
    const std::string &getName();

    void displayVars();

protected:
    std::string type;
    std::string name;
    std::map<std::string, double> vars;
    std::map<std::string, double*> parentvars;
    std::map<std::string, std::string> strings;
    std::vector<std::reference_wrapper<ScriptCommand> > commands;
    std::vector<std::unique_ptr<ScriptCommand> > tempcommands;

    //std::vector<std::variant<std::unique_ptr<ScriptBlock>, std::unique_ptr<ScriptCommand>>> ownedruntimecmds;
    bool ended;
    bool asloopblock;
    std::vector<std::reference_wrapper<ScriptBlock> > blocks;
    std::vector<std::unique_ptr<ScriptBlock> > ownedblocks;
    std::map<std::string, std::reference_wrapper<ScriptBlock> > functions;
    std::map<std::string, std::reference_wrapper<ScriptBlock> > parentfunctions;

    CurrenBlock current_block;
    ScriptBlock *loopblock;

    std::vector<std::variant<std::reference_wrapper<ScriptBlock>, std::reference_wrapper<ScriptCommand> > > commandsorder;
    std::vector<std::variant<std::reference_wrapper<ScriptBlock>, std::reference_wrapper<ScriptCommand> > > initOutLoopCommandsOrder;

    std::map<std::string, std::unique_ptr<Parser>> exprevaluated;

    int debug_times;
    bool stat;
    bool condchain_entered;

    std::size_t index_lastcmd;

    bool execInitCommands;
    bool resultLastCanEnter;

    bool last_could_enter;
    bool add_blockentry_test_on_getcmds;

};

#endif // SCRIPTBLOCK_H
