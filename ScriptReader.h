#ifndef SCRIPTREADER_H
#define SCRIPTREADER_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <variant>
#include <mpParser.h>

#include "MoveCommand.h"

class ScriptReader
{
public:
    ScriptReader(const std::string &path_script);
    virtual ~ScriptReader();
    void getCommands(std::size_t start, std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands);
    int nbCommands();
    const void* getVar(const std::string &var);

private:
    std::vector<std::shared_ptr<ScriptCommand>> commands;
    std::map<std::string, std::variant<void*>> vars;
    std::map<std::string, mup::Value> mupvars;
};

#endif // SCRIPTREADER_H
