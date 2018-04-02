#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <iostream>
#include <string>
#include <variant>
#include <map>

class ScriptCommand
{
public:
    ScriptCommand(const std::string &type);
    virtual ~ScriptCommand();
    const std::string &getType();
    const void *getProp(const std::string &name);
    const std::string &getStringProp(const std::string &name);

protected:
    std::string type;
    std::map<std::string, void*> props;
    std::map<std::string, std::string> strings;
};

#endif // SCRIPTCOMMAND_H
