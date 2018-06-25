#ifndef SCRIPTREADER_H
#define SCRIPTREADER_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <variant>
#include <vector>
#include <algorithm>

#include "ScriptBlock.h"
#include "MoveCommand.h"
#include "VarSetCommmand.h"
#include "IfBlock.h"
#include "ElseIfBlock.h"
#include "ElseBlock.h"
#include "WhileBlock.h"
#include "ForBlock.h"
#include "PrintCommand.h"
#include "FunctionBlock.h"
#include "Map.h"
#include "ShootMissileCommand.h"
#include "ShootGuidedMissile.h"
#include "GMissileTurnCommand.h"
#include "BlockEntryCommand.h"

const std::vector<std::string> internalFunctions = {"shootmissile", "shootguidedmissile"};

class ScriptReader
{
public:
    ScriptReader();
    virtual ~ScriptReader();
    void getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &commands);
    int nbCommands();
    double getVar(const std::string &var);
    void setVar(const std::string &var, double val);
    void addBlockCallFunc(const std::string &funcname, const std::vector<std::string> &exprs);
    void load(const std::string &path_script);

    void displayMainBlockVars();
    void initVars(const sf::Vector2f &pos, const Map &worldmap, float dt);

private:
    std::shared_ptr<ScriptBlock> mainblock;
    std::vector<std::shared_ptr<ScriptCommand>> commands;
    const static sf::Clock UniversalRobotsTime;
};

#endif // SCRIPTREADER_H
