#include "CreateBlockCommand.h"

CreateBlockCommand::CreateBlockCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &blockrel, const ParamVar &blockid) :
    ScriptCommand(block, "createblock")
{
    std::cout << "new create block !!" << std::endl;
    if (blockrel.type == CONSTANT)props.insert(std::pair<std::string, double>("blockrel", blockrel.doubleval));
    else{
        props.insert(std::pair<std::string, double>("blockrel", blockrel.doubleval));
        strings.insert(std::pair<std::string,
        std::string>("blockrel", blockrel.expr));
    }

    if (blockid.type == CONSTANT)props.insert(std::pair<std::string, double>("blockid", blockid.doubleval));
    else{
        props.insert(std::pair<std::string, double>("blockid", blockid.doubleval));
        strings.insert(std::pair<std::string,
        std::string>("blockid", blockid.expr));
    }
}

CreateBlockCommand::~CreateBlockCommand()
{
    //dtor
}

void CreateBlockCommand::update(){
    auto ptr = myblock.lock();
    if (ptr != nullptr){
        if (strings.find("blockrel") != strings.end()){
            double val = 0;
            if (ptr->varExist(strings["blockrel"])){
                val = ptr->getVar(strings["blockrel"]);
            }else ptr->evalParserExpr(strings["blockrel"], val);
            props["blockrel"] = val;
        }
        if (strings.find("blockid") != strings.end()){
            double val = 0;
            if (ptr->varExist(strings["blockid"])){
                val = ptr->getVar(strings["blockid"]);
            }else ptr->evalParserExpr(strings["blockid"], val);
            props["blockid"] = val;
        }
    }
}
