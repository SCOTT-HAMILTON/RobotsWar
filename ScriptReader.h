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
#include "VarSetCommand.h"
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
#include "DestroyBlockCommand.h"
#include "getGMissileXCommand.h"
#include "getGMissileYCommand.h"
#include "CreateBlockCommand.h"

const std::vector<std::string> internalFunctions =
{
    "shootmissile", "shootguidedmissile",
    "move", "turngmissileto", "destroyblock",
    "getgmissileX", "getgmissileY"
};

//Parsing Tools
bool extractParams(std::string &line, size_t nb_params, std::vector<ParamVar> &params);
void formateCondition(std::string &condition_expr);
void replace_all(std::string &s, const std::string &tosearch, const std::string &toreplace);
void parseFuncStr(const std::string &str, std::string &name, std::vector<std::string> &vars);
bool internalFunctionExist(const std::string &funcname);
std::vector<std::string> split(const std::string &text, char sep);

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

    //Parsing Tools
    bool functionExist(const std::string &funcname);
    void createFuncVarSet(const std::string &varname, const std::string &funcname, const std::vector<std::string> &vars);
    void createInternalFuncVarSet(const std::string &varname, const std::string &funcstr, size_t current_line);
    void parseInternalFunctions(bool &entered, std::string &line, bool &continuer);
    void parseLoops(bool &entered, std::string &line, bool &continuer);
    void parseConditionBlocks(bool &entered, std::string &line, bool &continuer);

private:
    std::shared_ptr<ScriptBlock> mainblock;
    std::vector<std::shared_ptr<ScriptCommand>> commands;
    const static sf::Clock UniversalRobotsTime;
    std::string touseline;
    std::shared_ptr<ScriptCommand> toaddcommand;
    size_t linetoaddcommand;
};

#endif // SCRIPTREADER_H
