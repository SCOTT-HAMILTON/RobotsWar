#include "MoveCommand.h"
#include <cmath>

MoveCommand::MoveCommand(offset x, offset y) :
    ScriptCommand("move"), offsetx(x), offsety(y)
{
    std::cout << "new move !!" << std::endl;
    if (offsetx.type == CONSTANT)props.insert(std::pair<std::string, vartype>("offsetx", offsetx.floatval));
    else{
        offsetx.floatval = NAN;
        props.insert(std::pair<std::string, vartype>("offsetx", offsetx.floatval));
        strings.insert(std::pair<std::string, std::string>("offsetx", offsetx.strval));
        std::cout << "strings test insertion : " << offsetx.strval << std::endl;
    }
    if (offsety.type == CONSTANT)props.insert(std::pair<std::string, vartype>("offsety", offsety.floatval));
    else{
        offsety.floatval = NAN;
        props.insert(std::pair<std::string, vartype>("offsety", offsety.floatval));
        strings.insert(std::pair<std::string, std::string>("offsety", offsety.strval));
        std::cout << "strings test insertion : " << offsety.strval << std::endl;
    }
}

MoveCommand::~MoveCommand()
{
    //dtor
}
