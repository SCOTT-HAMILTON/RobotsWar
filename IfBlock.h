#ifndef IFBLOCK_H
#define IFBLOCK_H

#include <iostream>
#include <string>

#include "ConditionBlock.h"

using idtype = unsigned long long;

class IfBlock : public ConditionBlock
{
public:
    IfBlock(const std::string &boolexpr);
    virtual ~IfBlock();
    virtual bool canEnter();
    virtual size_t getCommands(size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commands_ended);

protected:
    bool stillCan;
    size_t prec_indexlastcmd;

    static idtype curid;
    idtype myid;

    size_t prec_indexlastcommand;
};

#endif // IFBLOCK_H
