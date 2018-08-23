#include "getGMissileXCommand.h"

getGMissileXCommand::getGMissileXCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &id) :
    ScriptCommand(block, "getgmissilex"), myid(id)
{
    std::cout << "new guided missile get X pos command !!\n";
    if (myid.type == CONSTANT)props.insert(std::pair<std::string, double>("id", myid.doubleval));
    else{
        myid.doubleval = NAN;
        props.insert(std::pair<std::string, double>("id", myid.doubleval));
        strings.insert(std::pair<std::string, std::string>("id", myid.expr));
    }

    update();
}

getGMissileXCommand::~getGMissileXCommand()
{
    //dtor
}

void getGMissileXCommand::update(){
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
    }
}
