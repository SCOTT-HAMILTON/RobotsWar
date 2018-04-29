#include "PrintCommand.h"

PrintCommand::PrintCommand(std::weak_ptr<ScriptBlock> block, const std::vector<PrintElem> &elems) :
    ScriptCommand(block, "print")
{
    this->elems = elems;
    strings["str"] = "";
    update();
}

PrintCommand::~PrintCommand()
{
    //dtor
}

void PrintCommand::update(){
    std::string str("");
    auto ptr = myblock.lock();
    if (ptr != nullptr){
        for (std::size_t i = 0; i < elems.size(); i++){
            if (elems[i].type == STRING)str += elems[i].str;
            else{
                double val = 0;
                if (ptr->varExist(elems[i].str)){
                    val = ptr->getVar(elems[i].str);
                }else{
                    ptr->evalParserExpr(elems[i].str, val);
                }
                str += std::to_string(val);
            }
        }
    }else std::cout << "Error print command, block is nullptr !!! " << std::endl;
    strings["str"] = str;

}
