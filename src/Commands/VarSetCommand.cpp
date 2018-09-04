#include "Commands/VarSetCommand.h"

VarSetCommand::VarSetCommand(ScriptBlock &block, const std::string &_expr, const std::string &var) :
    ScriptCommand(block, "varset"), expr(_expr), varname(var)
{
    strings.insert(std::pair<std::string, std::string>("expr", expr));
    strings.insert(std::pair<std::string, std::string>("varname", varname));
    props.insert(std::pair<std::string, double>("val", 0));
}

VarSetCommand::VarSetCommand(VarSetCommand &varset) :
    ScriptCommand (varset.getBlock(), "varset"), expr(varset.expr), varname(varset.varname)
{
    strings.insert(std::pair<std::string, std::string>("expr", expr));
    strings.insert(std::pair<std::string, std::string>("varname", varname));
    props.insert(std::pair<std::string, double>("val", 0));
}

VarSetCommand::~VarSetCommand()
{
    //dtor
}

void VarSetCommand::setVarname(const std::string &_varname)
{
    varname = _varname;
    strings["varname"] = varname;
}

void VarSetCommand::setExpr(const std::string &_expr)
{
    expr = _expr;
    strings["expr"] = expr;
}

void VarSetCommand::update(){
    double val = 0;
    if (myblock.varExist(expr)){
        val = myblock.getVar(expr);
    }
    else {
        myblock.evalParserExpr(expr, val);
    }
    props["val"] = val;
    myblock.addVar(varname, val);



}
