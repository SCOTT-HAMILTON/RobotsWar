#include "ShootMissileCommand.h"

ShootMissileCommand::ShootMissileCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &x) :
    ScriptCommand(block, "shootmissile"), offsetx(x)
{
    std::cout << "new shootmisile !!\n";
    if (offsetx.type == CONSTANT)props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
    else{
        offsetx.doubleval = NAN;
        props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
        strings.insert(std::pair<std::string, std::string>("offsetx", offsetx.expr));
    }
}

ShootMissileCommand::~ShootMissileCommand()
{
    //dtor
}

void ShootMissileCommand::update(){
    auto ptr = myblock.lock();
    if (ptr != nullptr){
        if (strings.find("offsetx") != strings.end()){
            double val = 0;
            if (ptr->varExist(strings["offsetx"])){
                val = ptr->getVar(strings["offsetx"]);
            }else ptr->evalParserExpr(strings["offsetx"], val);
            props["offsetx"] = val;
        }
    }
}
