#include "Commands/MoveCommand.h"
#include <cmath>

MoveCommand::MoveCommand(ScriptBlock &block, const ParamVar &x, const ParamVar &y) :
    ScriptCommand(block, "move"), offsetx(x), offsety(y)
{
    std::cerr << "new move !!\n";
    if (offsetx.type == CONSTANT)props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
    else{
        offsetx.doubleval = static_cast<double>(NAN);
        props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
        strings.insert(std::pair<std::string, std::string>("offsetx", offsetx.expr));
    }
    if (offsety.type == CONSTANT)props.insert(std::pair<std::string, double>("offsety", offsety.doubleval));
    else{
        offsety.doubleval = static_cast<double>(NAN);
        props.insert(std::pair<std::string, double>("offsety", offsety.doubleval));
        strings.insert(std::pair<std::string, std::string>("offsety", offsety.expr));
    }

    update();
}

MoveCommand::~MoveCommand()
{
    //dtor
}

void MoveCommand::update(){
    if (strings.find("offsetx") != strings.end()){
        double val = 0;
        if (myblock.varExist(strings["offsetx"])){
            val = myblock.getVar(strings["offsetx"]);
        }else myblock.evalParserExpr(strings["offsetx"], val);
        props["offsetx"] = val;
    }
    if (strings.find("offsety") != strings.end()){
        double val = 0;
        if (myblock.varExist(strings["offsety"])){
            val = myblock.getVar(strings["offsety"]);
        }else myblock.evalParserExpr(strings["offsety"], val);
        props["offsety"] = val;
    }
}
