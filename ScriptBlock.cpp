#include "ScriptBlock.h"
#include <cstring>
#include <cstdlib>

ScriptBlock::ScriptBlock(const std::string &type, bool ended, std::map<std::string, double*> parentvars) :
    asloopblock(false)
{
    this->parentvars = parentvars;
    this->type = type;
    this->ended = ended;
    parser = std::make_shared<Parser>();
    current_block.index = 0;
    current_block.nb_in_wait = 0;
    debug_times = 0;
}

ScriptBlock::~ScriptBlock()
{
}

double ScriptBlock::getVar(const std::string &name){
    auto ptrblock = current_block.current_block.lock();
    if (ptrblock != nullptr)return ptrblock->getVar(name);
    if (parentvars.size() != 0 && parentvars.find(name) != parentvars.end())return *parentvars.at(name);
    if (vars.find(name) != vars.end())return vars[name];
    return double(0);
}

const std::string &ScriptBlock::getString(const std::string &name){
    auto ptrblock = current_block.current_block.lock();
    if (ptrblock != nullptr)return ptrblock->getString(name);
    if (strings.find(name) == strings.end())return "";
    return strings[name];
}

void ScriptBlock::addVar(const std::string &name, double var){
    auto ptr = current_block.current_block.lock() ;
    if (ptr != nullptr){
        ptr->addVar(name, var);
    }else{

        if (vars.find(name) != vars.end()){
            vars[name] = var;
        }else if (parentvars.find(name) != parentvars.end()){
            *parentvars[name] = var;
        }else{
            vars.emplace(name, var);
            std::cout << " BLOCK " << type << " ADDING VAR " << name << " TO VARS !!!!!!" << std::endl;
        }
    }
}

void ScriptBlock::addString(const std::string &name, const std::string &str){
    auto ptrblock = current_block.current_block.lock();
    if (ptrblock != nullptr)return ptrblock->addString(name, str);
    if (strings.find(name) != strings.end()){
        strings[name] = str;
    }else strings.insert(std::pair<std::string, std::string>(name, str));
}
void ScriptBlock::getCommands(std::size_t start, std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands){
    bool ismainblock = type == "mainblock";
    if (ismainblock){
        if (asloopblock){
            std::shared_ptr<ScriptBlock> block = loopblock.lock();
            if (block != nullptr){
                block->getCommands(start, nbCommands, pCommands);
            }
        }
    }else if (type == "loop"){
        for (std::size_t i = start; i < nbCommands+start && i < commands.size(); i++){
            pCommands.push_back( std::weak_ptr<ScriptCommand>(commands[i]) );
        }
    }
   // std::cout << "Block " << type << " commands : " << std::endl;
    for (auto t : pCommands){
        std::string ttype = t.lock()->getType();
       // std::cout << "command " << ttype << std::endl;
        if (ttype == "varset"){
            //std::cout << "varset : " << t.lock()->getProp("offsetx") << ", " << t.lock()->getStringProp("varname") << std::endl;
        }
    }
}

int ScriptBlock::nbCommands(){
    if (type == "mainblock" && asloopblock){
        std::shared_ptr<ScriptBlock> block = loopblock.lock();
        if (block != nullptr){
            return block->nbCommands();
        }
    }
    else if (type == "loop")return commands.size();
    return 0;
}

void ScriptBlock::addCommand(const std::weak_ptr<ScriptCommand> &command){
    std::shared_ptr<ScriptBlock> block = current_block.current_block.lock();
    if (block != nullptr){
        std::cout << "              NEW COMMAND " << command.lock()->getType() << " to " << block->getType() << std::endl;
        block->addCommand(command);
    }
    else {
        commands.push_back(command);
        std::cout << "              NEW COMMAND " << command.lock()->getType() << " to me " << type << std::endl;
    }
}

bool ScriptBlock::varExist(const std::string &name){
    auto ptrblock = current_block.current_block.lock();
    if (ptrblock != nullptr)return ptrblock->varExist(name);

    return (vars.find(name) != vars.end() || strings.find(name) != strings.end() || parentvars.find(name) != parentvars.end());
}

int ScriptBlock::evalParserExpr(const std::string &expr, double &val){
    auto ptblock = current_block.current_block.lock();
    if (ptblock != nullptr)return ptblock->evalParserExpr(expr, val);
    std::string varsstr("");
    std::size_t i = 0;
    std::size_t sizevarstr = vars.size();
    sizevarstr += parentvars.size();
    double tmpvars[sizevarstr];
    for (auto it = vars.begin(); it != vars.end(); it++){
        varsstr += it->first;
        auto it2 = it;
        it2++;
        if (it2 != vars.end())varsstr += ", ";
        tmpvars[i] = it->second;
        i++;
    }

    if (parentvars.size() != 0){
        if (varsstr != "")varsstr += ", ";
        for (auto it = parentvars.begin(); it != parentvars.end(); it++){
            varsstr += it->first;
            auto it2 = it;
            it2++;
            if (it2 != parentvars.end())varsstr += ", ";
            tmpvars[i] = *it->second;
            i++;
        }
    }

    int code = parser->Parse(expr, varsstr);
    if (code >= 0){
        std::cout << "error parsing expr " << expr << std::endl;
        std::cout << "error msg : " << parser->ErrorMsg() << std::endl;
        return 1;
    }



    val = parser->Eval(tmpvars);
    code = parser->EvalError();
    if (code > 0){
        std::cout << "error evaluanting expr " << expr << "error code : " << code <<  std::endl;
        return 1;
    }

    return 0;
}

void ScriptBlock::setEnded(){
    ended = true;
}

bool ScriptBlock::isEnded(){
    return ended;
}

void ScriptBlock::addBlock(const std::string &block){
    blocks.push_back( std::make_shared<ScriptBlock>(block) );
    for (auto it = vars.begin(); it != vars.end(); it++){
        blocks.back()->parentvars.emplace(it->first, &it->second);
    }
    if (parentvars.size() != 0){
        for (auto it = parentvars.begin(); it != parentvars.end(); it++){
            blocks.back()->parentvars.emplace(it->first, it->second);
        }
    }

    if (block == "loop"){
        loopblock = blocks.back();
        asloopblock = true;
    }
    current_block.current_block = blocks.back();
    current_block.index = block.size()-1;
    current_block.nb_in_wait++;

    std::cout << "          ADD BLOCK : " << blocks.back()->getType() << std::endl << std::endl;
}

void ScriptBlock::addBlockEnd(){
    std::shared_ptr ptr = current_block.current_block.lock();
    if (ptr != nullptr){
        std::cout << "      ENDING  BLOCK " << ptr->getType() << std::endl;
        ptr->setEnded();
        if (ptr->isEnded())current_block.nb_in_wait--;
        current_block.current_block = std::shared_ptr<ScriptBlock>();
        for (int i = blocks.size()-1; i >= 0; i--){
            if (!blocks[i]->isEnded()){
                current_block.index = i;
                current_block.current_block = blocks[i];
                break;
            }
        }
    }

}

bool ScriptBlock::allBlocksEnded(){
    for (std::size_t i = 0; i < blocks.size(); i++){
        if (!blocks[i]->isEnded())return false;
    }
    return true;
}

const std::string &ScriptBlock::getType(){
    return type;
}

bool ScriptBlock::canEnter(){
    return true;
}

std::weak_ptr<Parser> ScriptBlock::getParser(){
    return parser;
}

void ScriptBlock::displayVars(){
}

std::weak_ptr<ScriptBlock> ScriptBlock::getCurBlock(){
    return current_block.current_block;
}
