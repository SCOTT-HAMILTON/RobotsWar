#include "MoveCommand.h"
#include <cmath>

MoveCommand::MoveCommand(std::weak_ptr<ScriptBlock> block, offset x, offset y) :
    ScriptCommand(block, "move"), offsetx(x), offsety(y)
{
    std::cout << "new move !!" << std::endl;
    if (offsetx.type == CONSTANT)props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
    else{
        offsetx.doubleval = NAN;
        props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
        strings.insert(std::pair<std::string, std::string>("offsetx", offsetx.expr));
    }
    if (offsety.type == CONSTANT)props.insert(std::pair<std::string, double>("offsety", offsety.doubleval));
    else{
        offsety.doubleval = NAN;
        props.insert(std::pair<std::string, double>("offsety", offsety.doubleval));
        strings.insert(std::pair<std::string, std::string>("offsety", offsety.expr));
    }

    update();
}

MoveCommand::~MoveCommand()
{
    //dtor
}

void MoveCommand::update(){
    auto ptr = myblock.lock();
    if (ptr != nullptr){
        if (strings.find("offsetx") != strings.end()){
            double val = 0;
            ptr->evalParserExpr(strings["offsetx"], val);
            props["offsetx"] = val;
        }
        if (strings.find("offsety") != strings.end()){
            double val = 0;
            ptr->evalParserExpr(strings["offsety"], val);
            props["offsety"] = val;
        }
    }
}
