#include "Commands/CreateBlockCommand.h"

CreateBlockCommand::CreateBlockCommand(ScriptBlock &block, const ParamVar &blockrel, const ParamVar &blockid) :
    ScriptCommand(block, "createblock")
{
    std::cerr << "new create block !!\n";
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
    if (strings.find("blockrel") != strings.end()){
        double val = 0;
        if (myblock.varExist(strings["blockrel"])){
            val = myblock.getVar(strings["blockrel"]);
        }else myblock.evalParserExpr(strings["blockrel"], val);
        props["blockrel"] = val;
    }
    if (strings.find("blockid") != strings.end()){
        double val = 0;
        if (myblock.varExist(strings["blockid"])){
            val = myblock.getVar(strings["blockid"]);
        }else myblock.evalParserExpr(strings["blockid"], val);
        props["blockid"] = val;
    }
}
