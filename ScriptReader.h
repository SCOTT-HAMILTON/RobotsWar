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

using vartype = std::variant<float>;

class ScriptReader
{
public:
    ScriptReader(const std::string &path_script);
    virtual ~ScriptReader();
    void getCommands(std::size_t start, std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands);
    int nbCommands();
    const vartype &getVar(const std::string &var);
    void setVar(const std::string &var, float val);

    void displayMainBlockVars();

private:
    std::vector<std::shared_ptr<ScriptCommand>> commands;
    std::map<std::string, vartype> vars;
    std::shared_ptr<ScriptBlock> mainblock;
};

#endif // SCRIPTREADER_H
