#ifndef SCRIPTREADER_H
#define SCRIPTREADER_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <variant>

#include "ScriptBlock.h"
#include "MoveCommand.h"
#include "VarSetCommmand.h"
#include "IfBlock.h"
#include "ElseIfBlock.h"
#include "ElseBlock.h"
#include "WhileBlock.h"
#include "ForBlock.h"

class ScriptReader
{
public:
    ScriptReader(const std::string &path_script);
    virtual ~ScriptReader();
    void getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands);
    int nbCommands();
    double getVar(const std::string &var);
    void setVar(const std::string &var, double val);

    void displayMainBlockVars();

private:
    std::vector<std::shared_ptr<ScriptCommand>> commands;
    std::shared_ptr<ScriptBlock> mainblock;
};

#endif // SCRIPTREADER_H
