#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <iostream>
#include <string>
#include <variant>
#include <map>

using vartype = std::variant<float>;

class ScriptCommand
{
public:
    ScriptCommand(const std::string &type);
    virtual ~ScriptCommand();
    const std::string &getType();
    const vartype &getProp(const std::string &name);
    const std::string &getStringProp(const std::string &name);
    virtual void update(){}

protected:
    std::string type;
    std::map<std::string, vartype> props;
    std::map<std::string, std::string> strings;
};

#endif // SCRIPTCOMMAND_H
