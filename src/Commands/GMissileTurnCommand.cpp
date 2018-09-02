#include "Commands/GMissileTurnCommand.h"

GMissileTurnCommand::GMissileTurnCommand(ScriptBlock &block, const ParamVar &id, const ParamVar &val) :
    ScriptCommand(block, "gmissileturn"), myid(id), myangle(val)
{
    std::cerr << "new guided missile turn to command !!\n";
    if (myid.type == CONSTANT)props.insert(std::pair<std::string, double>("id", myid.doubleval));
    else{
        myid.doubleval = static_cast<double>(NAN);
        props.insert(std::pair<std::string, double>("id", myid.doubleval));
        strings.insert(std::pair<std::string, std::string>("id", myid.expr));
    }
    if (myangle.type == CONSTANT)props.insert(std::pair<std::string, double>("angle", myangle.doubleval));
    else{
        myangle.doubleval = static_cast<double>(NAN);
        props.insert(std::pair<std::string, double>("angle", myangle.doubleval));
        strings.insert(std::pair<std::string, std::string>("angle", myangle.expr));
    }

    update();
}

GMissileTurnCommand::~GMissileTurnCommand()
{
    //dtor
}

void GMissileTurnCommand::update(){
    if (strings.find("id") != strings.end()){
        double val = 0;
        if (myblock.varExist(strings["id"])){
            val = myblock.getVar(strings["id"]);
        }else {
            myblock.evalParserExpr(strings["id"], val);
        }

        props["id"] = val;
    }
    if (strings.find("angle") != strings.end()){
        double val = 0;
        if (myblock.varExist(strings["angle"])){
            val = myblock.getVar(strings["angle"]);
        }else myblock.evalParserExpr(strings["angle"], val);
        props["angle"] = val;
    }
}
