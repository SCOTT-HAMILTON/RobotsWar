#include "DestroyBlockCommand.h"

DestroyBlockCommand::DestroyBlockCommand(std::weak_ptr<ScriptBlock> block, const ParamVar &blockparam) :
    ScriptCommand(block, "destroyblock")
{
    std::cout << "new destroy block !!\n";
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
    auto ptr = myblock.lock();
    if (ptr != nullptr){
        if (strings.find("block") != strings.end()){
            double val = 0;
            if (ptr->varExist(strings["block"])){
                val = ptr->getVar(strings["block"]);
            }else ptr->evalParserExpr(strings["block"], val);
            props["block"] = val;
        }
    }
}
