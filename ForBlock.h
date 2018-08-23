#ifndef FORBLOCK_H
#define FORBLOCK_H

#include <iostream>

#include "ConditionBlock.h"
#include "BlockEntryCommand.h"

using idtype = unsigned long long;

class ForBlock : public ConditionBlock
{
public:
    ForBlock(const std::string &var, const std::string &boolexpr, const std::string &startexpr, const std::string &exprincremente);
    virtual ~ForBlock();
    virtual bool canEnter();
    virtual bool getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands, bool &commandsended);

private:
    std::string incrementer;
    std::string varname;
    std::string startexpr;
    double getValFromExpr(const std::string &expr);
    idtype myid;
    static idtype curid;
    bool all_cmds_done;
    bool first;
    std::shared_ptr<BlockEntryCommand> cmd_last_eval_entry;
};

#endif // FORBLOCK_H
