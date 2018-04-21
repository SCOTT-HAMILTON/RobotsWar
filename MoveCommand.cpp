#include "MoveCommand.h"
#include <cmath>

MoveCommand::MoveCommand(std::shared_ptr<ScriptBlock> block, offset x, offset y) :
    ScriptCommand(block, "move"), offsetx(x), offsety(y)
{
    std::cout << "new move !!" << std::endl;
    if (offsetx.type == CONSTANT)props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
    else{
        offsetx.doubleval = NAN;
        props.insert(std::pair<std::string, double>("offsetx", offsetx.doubleval));
        strings.insert(std::pair<std::string, std::string>("offsetx", offsetx.strval));
    }
    if (offsety.type == CONSTANT)props.insert(std::pair<std::string, double>("offsety", offsety.doubleval));
    else{
        offsety.doubleval = NAN;
        props.insert(std::pair<std::string, double>("offsety", offsety.doubleval));
        strings.insert(std::pair<std::string, std::string>("offsety", offsety.strval));
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
            props["offsetx"] = ptr->getVar(strings["offsetx"]);
            //std::cout << "off x var name : " << strings["offsetx"] << " value : " << props["offsetx"] << std::endl;
        }
        if (strings.find("offsety") != strings.end()){
            props["offsety"] = ptr->getVar(strings["offsety"]);
        }
    }
}
