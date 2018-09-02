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
#include <array>
#include <algorithm>

#include "Blocks/ScriptBlock.h"
#include "Blocks/MainBlock.h"
#include "Blocks/FunctionBlock.h"
#include "Blocks/IfBlock.h"
#include "Blocks/ElseIfBlock.h"
#include "Blocks/ElseBlock.h"
#include "Blocks/WhileBlock.h"
#include "Blocks/ForBlock.h"
#include "Blocks/CallFuncBlock.h"

#include "Commands/MoveCommand.h"
#include "Commands/VarSetCommand.h"
#include "Commands/PrintCommand.h"
#include "Commands/ShootMissileCommand.h"
#include "Commands/ShootGuidedMissileCommand.h"
#include "Commands/GMissileTurnCommand.h"
#include "Commands/BlockEntryCommand.h"
#include "Commands/DestroyBlockCommand.h"
#include "Commands/getGMissileXCommand.h"
#include "Commands/getGMissileYCommand.h"
#include "Commands/CreateBlockCommand.h"

#include "Rendering/Map.h"

static const std::array<std::string, 7> internalFunctions{
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
    void getCommands(std::size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &commands);
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
    bool parseInternalFunctions(bool &entered, std::string &line, bool &continuer);
    bool parseLoops(bool &entered, std::string &line, bool &continuer);
    bool parseConditionBlocks(bool &entered, std::string &line);

private:
    std::unique_ptr<MainBlock> mainblock;
    std::vector<std::unique_ptr<ScriptCommand>> commands;// storage of all commands created in parser (to avoid nullptr)
    std::vector<std::unique_ptr<ScriptBlock>> blocks; // storage of all blocks created in parser (to avoid nullptr)
    const static sf::Clock UniversalRobotsTime;
    std::string touseline;
    std::unique_ptr<ScriptCommand> toaddcommand;
    size_t linetoaddcommand;
};

#endif // SCRIPTREADER_H
