#include "Commands/PrintCommand.h"

PrintCommand::PrintCommand(ScriptBlock &block, const std::vector<PrintElem> &_elems) :
    ScriptCommand(block, "print"), elems(_elems)
{
    strings["str"] = "";
    update();
}

PrintCommand::~PrintCommand()
{
    //dtor
}

void PrintCommand::update(){
    std::string str("");
    for (std::size_t i = 0; i < elems.size(); i++){
        if (elems[i].type == STRING)str += elems[i].str;
        else{
            double val = 0;
            if (myblock.varExist(elems[i].str)){
                val = myblock.getVar(elems[i].str);
            }else{
                myblock.evalParserExpr(elems[i].str, val);
            }
            str += std::to_string(val);
        }
    }
    strings["str"] = str;

}
