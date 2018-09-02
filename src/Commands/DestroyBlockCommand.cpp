#include "Commands/DestroyBlockCommand.h"

DestroyBlockCommand::DestroyBlockCommand(ScriptBlock &block, const ParamVar &blockparam) :
    ScriptCommand(block, "destroyblock")
{
    std::cerr << "new destroy block !!\n";
    if (blockparam.type == CONSTANT)props.insert(std::pair<std::string, double>("block", blockparam.doubleval));
    else{
        props.insert(std::pair<std::string, double>("block", blockparam.doubleval));
        strings.insert(std::pair<std::string,
        std::string>("block", blockparam.expr));
    }
}

DestroyBlockCommand::~DestroyBlockCommand()
{
    //dtor
}

void DestroyBlockCommand::update(){
    if (strings.find("block") != strings.end()){
        double val = 0;
        if (myblock.varExist(strings["block"])){
            val = myblock.getVar(strings["block"]);
        }else myblock.evalParserExpr(strings["block"], val);
        props["block"] = val;
    }
}
