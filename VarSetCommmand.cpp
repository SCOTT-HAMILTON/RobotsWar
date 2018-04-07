#include "VarSetCommmand.h"

VarSetCommmand::VarSetCommmand(std::shared_ptr<ScriptBlock> block, const std::string &expr, const std::string &var) :
    ScriptCommand("varset"), myblock(block), expr(expr), varname(var)
{
    strings.insert(std::pair<std::string, std::string>("expr", expr));
    strings.insert(std::pair<std::string, std::string>("varname", varname));
    props.insert(std::pair<std::string, vartype>("val", value));
    update();
}

VarSetCommmand::~VarSetCommmand()
{
    //dtor
}

void VarSetCommmand::update(){
    std::shared_ptr<ScriptBlock> ptrblock = myblock.lock();
    if (ptrblock != nullptr){
        float val;
        ptrblock->evalParserExpr(expr, val);
        props["val"] = val;

    }else std::cout << "ptrblock is nullptr" << std::endl;

}
