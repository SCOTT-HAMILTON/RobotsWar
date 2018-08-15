#include "GMissileTurnCommand.h"

GMissileTurnCommand::GMissileTurnCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &id, const ParamVar &val) :
    ScriptCommand(block, "gmissileturn"), myid(id), myangle(val)
{
    std::cout << "new guided missile turn to command !!\n";
    if (myid.type == CONSTANT)props.insert(std::pair<std::string, double>("id", myid.doubleval));
    else{
        myid.doubleval = NAN;
        props.insert(std::pair<std::string, double>("id", myid.doubleval));
        strings.insert(std::pair<std::string, std::string>("id", myid.expr));
    }
    if (myangle.type == CONSTANT)props.insert(std::pair<std::string, double>("angle", myangle.doubleval));
    else{
        myangle.doubleval = NAN;
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
    auto ptr = myblock.lock();
    if (ptr != nullptr){
        if (strings.find("id") != strings.end()){
            double val = 0;
            if (ptr->varExist(strings["id"])){
                val = ptr->getVar(strings["id"]);
            }else {
                ptr->evalParserExpr(strings["id"], val);
            }

            props["id"] = val;
        }
        if (strings.find("angle") != strings.end()){
            double val = 0;
            if (ptr->varExist(strings["angle"])){
                val = ptr->getVar(strings["angle"]);
            }else ptr->evalParserExpr(strings["angle"], val);
            props["angle"] = val;
        }
    }
}
