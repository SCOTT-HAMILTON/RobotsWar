#include "Commands/getGMissileYCommand.h"

getGMissileYCommand::getGMissileYCommand(ScriptBlock &block, const ParamVar &id) :
    ScriptCommand(block, "getgmissiley"), myid(id)
{
    std::cerr << "new guided missile get X pos command !!\n";
    if (myid.type == CONSTANT)props.insert(std::pair<std::string, double>("id", myid.doubleval));
    else{
        myid.doubleval = static_cast<double>(NAN);
        props.insert(std::pair<std::string, double>("id", myid.doubleval));
        strings.insert(std::pair<std::string, std::string>("id", myid.expr));
    }

    update();
}

getGMissileYCommand::~getGMissileYCommand()
{
    //dtor
}

void getGMissileYCommand::update(){
    if (strings.find("id") != strings.end()){
        double val = 0;
        if (myblock.varExist(strings["id"])){
            val = myblock.getVar(strings["id"]);
        }else {
            myblock.evalParserExpr(strings["id"], val);
        }

        props["id"] = val;
    }
}
