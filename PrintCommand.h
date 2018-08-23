#ifndef PRINTCOMMAND_H
#define PRINTCOMMAND_H

#include <iostream>
#include <vector>
#include <string>

#include "ScriptCommand.h"

enum PrintElemType{STRING, EXPR};

struct PrintElem{
    std::string str;
    PrintElemType type;
};

class PrintCommand : public ScriptCommand
{
public:
    PrintCommand(std::weak_ptr<ScriptBlock> block, const std::vector<PrintElem> &elems);
    virtual ~PrintCommand();
    virtual void update();

private:
    std::vector<PrintElem> elems;
};

#endif // PRINTCOMMAND_H
