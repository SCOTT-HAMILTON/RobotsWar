#include "VarSetCommand.h"

VarSetCommand::VarSetCommand(std::weak_ptr<ScriptBlock> block, const std::string &expr, const std::string &var) :
    ScriptCommand(block, "varset"), expr(expr), varname(var)
{
    std::cout << "new varset : " << varname << " = " << expr << "\n";
    strings.insert(std::pair<std::string, std::string>("expr", expr));
    strings.insert(std::pair<std::string, std::string>("varname", varname));
    props.insert(std::pair<std::string, double>("val", value));
}

VarSetCommand::~VarSetCommand()
{
    //dtor
}

void VarSetCommand::update(){
    std::shared_ptr<ScriptBlock> ptrblock = myblock.lock();
    if (ptrblock != nullptr){
        double val;
        if (ptrblock->varExist(expr)){
            val = ptrblock->getVar(expr);
        }
        else {
            ptrblock->evalParserExpr(expr, val);
        }

        props["val"] = val;
        value = val;
        ptrblock->addVar(varname, val);
    }else std::cout << "ptrblock is nullptr\n";

}
