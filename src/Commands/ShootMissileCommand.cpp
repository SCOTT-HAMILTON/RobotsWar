#include "Commands/ShootMissileCommand.h"

ShootMissileCommand::ShootMissileCommand(ScriptBlock &block, const ParamVar &x) :
    ScriptCommand(block, "shootmissile"), offsetx(x)
{
    std::cerr << "new shootmisile !!\n";
    if (offsetx.type == CONSTANT)props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
    else{
        offsetx.doubleval = static_cast<double>(NAN);
        props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
        strings.insert(std::pair<std::string, std::string>("offsetx", offsetx.expr));
    }
}

ShootMissileCommand::~ShootMissileCommand()
{
    //dtor
}

void ShootMissileCommand::update(){
    if (strings.find("offsetx") != strings.end()){
        double val = 0;
        if (myblock.varExist(strings["offsetx"])){
            val = myblock.getVar(strings["offsetx"]);
        }else myblock.evalParserExpr(strings["offsetx"], val);
        props["offsetx"] = val;
    }
}
