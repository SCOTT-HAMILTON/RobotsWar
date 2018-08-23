#ifndef WHILEBLOCK_H
#define WHILEBLOCK_H

#include <iostream>

#include "ConditionBlock.h"

class WhileBlock : public ConditionBlock
{
public:
    WhileBlock(const std::string &boolexpr);
    virtual ~WhileBlock();
    virtual bool canEnter();
    virtual bool getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands, bool &commandsended);

private:
    std::shared_ptr<bool> resultLastEntryTest;

};

#endif // WHILEBLOCK_H
