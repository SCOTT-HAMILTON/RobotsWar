#include "ScriptBlock.h"
#include <cstring>
#include <cstdlib>

ScriptBlock::ScriptBlock(const std::string &type, bool ended) :
    asloopblock(false), index_lastcmd(0)
{
    this->parentvars = parentvars;
    this->type = type;
    this->ended = ended;
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
std::size_t ScriptBlock::getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands, bool &commandsended){
    bool ismainblock = type == "mainblock";
    commandsended = false;
    std::size_t commands_done = 0;
    std::size_t size_pcommandsstart = pCommands.size();
    if (ismainblock){
        if (asloopblock){
            std::shared_ptr<ScriptBlock> block = loopblock.lock();
            if (block != nullptr && block->canEnter()){
                block->getCommands(nbCommands, pCommands, commandsended);
            }
        }
    }else {
        if (index_lastcmd>=commandsorder.size())index_lastcmd = 0;
        bool last_ended_properly = true;
        for (std::size_t i = index_lastcmd; i < commandsorder.size() && commands_done<nbCommands; i++){
            last_ended_properly = true;
            try{
                auto block = std::get<std::weak_ptr<ScriptBlock>>(commandsorder[i]);
                auto ptr = block.lock();
                if (ptr != nullptr){
                    if (ptr->canEnter()){
                        commands_done += ptr->getCommands(nbCommands-commands_done, pCommands, last_ended_properly);
                    }
                }else{
                    std::cout << "block ptr of commands order is nullptr !!" << std::endl;
                }
            }
            catch (const std::bad_variant_access&) {
                auto cmd = std::get<std::weak_ptr<ScriptCommand>>(commandsorder[i]);
                auto ptr = cmd.lock();
                if (ptr != nullptr){
                    pCommands.push_back(cmd);
                    commands_done++;
                    if (ptr->getType() == "varset"){
                        ptr->update();
                        addVar(ptr->getStringProp("varname"), ptr->getProp("val"));
                    }
                }else{
                }
            }
            index_lastcmd = i;
        }
        if (last_ended_properly)index_lastcmd++;
        if (index_lastcmd>=commandsorder.size()){
            index_lastcmd = 0;
            commandsended = true;
        }
    }
    return commands_done;
}

int ScriptBlock::nbCommands(){
    int nb = 0;
    if (type == "mainblock" && asloopblock){
        auto ptr = loopblock.lock();
        if (ptr != nullptr)return ptr->nbCommands();
    }else{
        nb = commands.size();
        for (std::size_t i = 0; i < blocks.size(); i++){
            nb += blocks[i]->nbCommands();
        }
    }
    return nb;
}

void ScriptBlock::addCommand(const std::weak_ptr<ScriptCommand> &command){
    std::shared_ptr<ScriptBlock> block = current_block.current_block.lock();
    if (block != nullptr){
        std::cout << "              NEW COMMAND " << command.lock()->getType() << " to " << block->getType() << std::endl;
        block->addCommand(command);
    }
    else {
        commands.push_back(command);
        commandsorder.push_back(commands.back());
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

    //if (type == "forblock") std::cout << "varstr : " << varsstr << std::endl;

    int code;
    std::shared_ptr<Parser> ptrparser;
    if (exprevaluated.find(expr) != exprevaluated.end()){
        ptrparser = exprevaluated[expr];
    }
    else {
        std::cout << "new parser!!" << std::endl;
        ptrparser = std::make_shared<Parser>();
        exprevaluated.emplace(expr, ptrparser);
        code = ptrparser->Parse(expr, varsstr);
        if (code >= 0){
            std::cout << "error parsing expr " << expr << std::endl;
            std::cout << "error msg : " << ptrparser->ErrorMsg() << std::endl;
            return 1;
        }
        ptrparser->Optimize();
    }

    val = ptrparser->Eval(tmpvars);
    code = ptrparser->EvalError();
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
    std::shared_ptr ptr = current_block.current_block.lock();
    if (ptr != nullptr){
        ptr->addBlock(block);
    }else{
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
        current_block.index = blocks.size()-1;
        current_block.nb_in_wait++;

        commandsorder.push_back(blocks.back());

        std::cout << "          ADD BLOCK : " << blocks.back()->getType() << std::endl << std::endl;

        //vars test

        std::string varsstr("");
        std::size_t i = 0;
        for (auto it = blocks.back()->vars.begin(); it != blocks.back()->vars.end(); it++){
            varsstr += it->first;
            auto it2 = it;
            it2++;
            if (it2 != blocks.back()->vars.end())varsstr += ", ";
        }
        std::cout << "VARS : " << varsstr << std::endl;

        varsstr = "";

        if (blocks.back()->parentvars.size() != 0){
            if (varsstr != "")varsstr += ", ";
            for (auto it = blocks.back()->parentvars.begin(); it != blocks.back()->parentvars.end(); it++){
                varsstr += it->first;
                auto it2 = it;
                it2++;
                if (it2 != blocks.back()->parentvars.end())varsstr += ", ";
            }
        }
        std::cout << "PARENTVARS : " << varsstr << std::endl;

    }
}

void ScriptBlock::addBlock(std::shared_ptr<ScriptBlock> block){
    std::shared_ptr ptr = current_block.current_block.lock();
    if (ptr != nullptr){
        ptr->addBlock(block);
    }else{
        blocks.push_back(block);
        for (auto it = vars.begin(); it != vars.end(); it++){
            blocks.back()->parentvars.emplace(it->first, &it->second);
        }
        if (parentvars.size() != 0){
            for (auto it = parentvars.begin(); it != parentvars.end(); it++){
                blocks.back()->parentvars.emplace(it->first, it->second);
            }
        }

        if (block->getType() == "loop"){
            loopblock = blocks.back();
            asloopblock = true;
        }
        current_block.current_block = blocks.back();
        current_block.index = blocks.size()-1;
        current_block.nb_in_wait++;

        commandsorder.push_back(blocks.back());
    }
}

void ScriptBlock::addBlockEnd(){
    std::shared_ptr ptr = current_block.current_block.lock();
    if (ptr != nullptr){
        ptr->addBlockEnd();
        if (ptr->isEnded())current_block.nb_in_wait--;
        current_block.current_block = std::shared_ptr<ScriptBlock>();
        for (int i = blocks.size()-1; i >= 0; i--){
            if (!blocks[i]->isEnded()){
                current_block.index = i;
                current_block.current_block = blocks[i];
                break;
            }
        }
    }else{
        std::cout << "      ENDING BLOCK " << type << std::endl;
        setEnded();
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

void ScriptBlock::displayVars(){
}

std::weak_ptr<ScriptBlock> ScriptBlock::getCurBlock(){
    auto ptr = current_block.current_block.lock();
    if (ptr != nullptr){
        auto tmpptr = ptr->getCurBlock();
        if (tmpptr.lock() == nullptr){
            return ptr;
        }
        return tmpptr;
    }
    return ptr;
}

bool ScriptBlock::getStat(){
    return stat;
}

bool ScriptBlock::isCondChainEntered(){
    return condchain_entered;
}


std::weak_ptr<ScriptBlock> ScriptBlock::getLastEndedBlock(){
    auto ptr = current_block.current_block.lock();
    std::cout << "sfkdsjfkl type " << type  << std::endl;
    if (ptr != nullptr){
        return ptr->getLastEndedBlock();
    }
    return blocks.back();
}
