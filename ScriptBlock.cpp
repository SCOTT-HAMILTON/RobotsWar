#include "ScriptBlock.h"
#include "BlockEntryCommand.h"
#include <cstring>
#include <cstdlib>

ScriptBlock::ScriptBlock(const std::string &type, bool ended) :
    asloopblock(false), index_lastcmd(0), execInitCommands(false)
{
    name = type;
    this->parentvars = parentvars;
    this->type = type;
    this->ended = ended;
    current_block.nb_in_wait = 0;
    debug_times = 0;

    if (type.rfind("call", 0) != 0)addVar("return", 0);
    else std::cout << "function call no personal return !!" << std::endl;
}

ScriptBlock::~ScriptBlock()
{
}

double ScriptBlock::getVar(const std::string &name){
    auto ptrblock = current_block.current_block.lock();
    if (ptrblock != nullptr)return ptrblock->getVar(name);
    if (parentvars.find(name) != parentvars.end())return *parentvars.at(name);
    if (vars.find(name) != vars.end())return vars.at(name);

    return double(0);
}

double *ScriptBlock::getVarPtr(const std::string &name){
    std::cout << "               ASKING FOR VAR PTR " << name << " block " << type << std::endl;
    auto ptrblock = current_block.current_block.lock();
    if (ptrblock != nullptr)return ptrblock->getVarPtr(name);
    if (parentvars.find(name) != parentvars.end())return parentvars.at(name);
    if (vars.find(name) != vars.end())return &vars.at(name);

    return nullptr;
}

double *ScriptBlock::getPersonalVarPtr(const std::string &name){
    if (parentvars.find(name) != parentvars.end())return parentvars.at(name);
    if (vars.find(name) != vars.end())return &vars.at(name);

    return nullptr;
}

std::string ScriptBlock::getVarName(std::size_t index){
    auto ptrblock = current_block.current_block.lock();
    if (ptrblock != nullptr)return ptrblock->getVarName(index);
    std::size_t i = 0;
    if (index>=vars.size()){
        std::cout << "error index too big !! index : " << index << ", varssize : " << vars.size() << ", type : " << type << std::endl;
    }else {
        for (auto it = vars.begin(); it != vars.end(); it++){
            if (i == index)return it->first;
            i++;
        }

    }
    return "";
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
        }
    }
}

void ScriptBlock::addParentVar(const std::string &name, double *var){
    auto ptr = current_block.current_block.lock() ;
    if (ptr != nullptr){
        ptr->addParentVar(name, var);
    }else{
        parentvars[name] = var;
        std::cout << "new parentvar block " << type << " : " << name << " val " << *var << std::endl;
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
    tempcommands.clear();
    bool launched = false;
    std::vector<std::variant<std::weak_ptr<ScriptBlock>, std::weak_ptr<ScriptCommand>>> *commands_to_exec = &commandsorder;

    if (ismainblock){
        if (asloopblock){
            if (!execInitCommands){
                commands_to_exec = &initOutLoopCommandsOrder;
            }
            else{
                std::shared_ptr<ScriptBlock> block = loopblock.lock();
                if (block != nullptr && block->canEnter()){
                    block->getCommands(nbCommands, pCommands, commandsended);
                    launched = true;
                }
            }
        }
    }
    if (!launched){

        if (index_lastcmd>=commands_to_exec->size())index_lastcmd = 0;
        bool last_ended_properly = true;
        for (std::size_t i = index_lastcmd; i < commands_to_exec->size() && commands_done<nbCommands; i++){
            last_ended_properly = true;
            try{
                auto block = std::get<std::weak_ptr<ScriptBlock>>(commands_to_exec->at(i));
                auto ptr = block.lock();
                if (ptr != nullptr){
                    if (ptr->getType() == "functionblock");
                    else{
                        tempcommands.push_back( std::make_shared<BlockEntryCommand>(ptr, 0) );
                        pCommands.push_back(tempcommands.back());
                        std::size_t before = pCommands.size();
                        commands_done += ptr->getCommands(nbCommands-commands_done, pCommands, last_ended_properly);
                        tempcommands.back()->setProp("nbcmd", pCommands.size()-before);
                    }
                }else{
                    std::cout << "block ptr of commands order is nullptr !!" << std::endl;
                }
            }
            catch (const std::bad_variant_access&) {
                auto cmd = std::get<std::weak_ptr<ScriptCommand>>(commands_to_exec->at(i));
                auto ptr = cmd.lock();
                if (ptr != nullptr){
                    pCommands.push_back(cmd);
                    commands_done++;
                }else{
                }
            }
            index_lastcmd = i;
        }
        if (last_ended_properly)index_lastcmd++;
        if (index_lastcmd>=commands_to_exec->size()){
            index_lastcmd = 0;
            if (commands_to_exec == &initOutLoopCommandsOrder){
                execInitCommands = true;
            }
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
        for (std::size_t i = 0; i < commandsorder.size(); i++){
            try{
                auto ptr = std::get<std::weak_ptr<ScriptCommand>>(commandsorder[i]);
                nb++;
            }catch (std::exception &err){
                auto ptr = std::get<std::weak_ptr<ScriptBlock>>(commandsorder[i]).lock();
                if (ptr != nullptr)nb += ptr->nbCommands();
                else std::cout << "error nbcommands block is nullptr !!" << std::endl;
            }
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
        commandsorder.push_back(command);
        if (type == "mainblock" && !asloopblock){
            initOutLoopCommandsOrder.push_back(command);
        }
        std::cout << "              NEW COMMAND " << command.lock()->getType() << " to me " << type << std::endl;
    }
}

void ScriptBlock::addNotPlayedCommand(const std::weak_ptr<ScriptCommand> &command){
    std::shared_ptr<ScriptBlock> block = current_block.current_block.lock();
    if (block != nullptr){
        block->addNotPlayedCommand(command);
    }else {
        commands.push_back(command);
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

    int code;
    std::shared_ptr<Parser> ptrparser;
    if (exprevaluated.find(expr+varsstr) != exprevaluated.end()){
        ptrparser = exprevaluated[expr+varsstr];
    }
    else {
        ptrparser = std::make_shared<Parser>();
        exprevaluated.emplace(expr+varsstr, ptrparser);
        code = ptrparser->Parse(expr, varsstr);
        if (code >= 0){
            std::cout << "error parsing expr " << expr << std::endl;
            std::cout << "me : " << type << ", vars : " << varsstr << std::endl;
            std::cout << "error msg : " << ptrparser->ErrorMsg() << std::endl;
            return 1;
        }
        //ptrparser->Optimize();
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
            if (it->first != "return")blocks.back()->parentvars.emplace(it->first, &it->second);
        }
        if (parentvars.size() != 0){
            for (auto it = parentvars.begin(); it != parentvars.end(); it++){
                if (it->first != "return")blocks.back()->parentvars.emplace(it->first, it->second);
            }
        }

        for (auto it = functions.begin(); it != functions.end(); it++){
            blocks.back()->parentfunctions.emplace(it->first, it->second);
        }

        for (auto it = parentfunctions.begin(); it != parentfunctions.end(); it++){
            blocks.back()->parentfunctions.emplace(it->first, it->second);
        }


        if (block == "loop"){
            loopblock = blocks.back();
            asloopblock = true;
        }
        current_block.current_block = blocks.back();
        current_block.nb_in_wait++;

        commandsorder.push_back(blocks.back());
        if (type == "mainblock" && !asloopblock){
            initOutLoopCommandsOrder.push_back(blocks.back());
        }

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
        if (block->type == "functionblock")functions.emplace(block->name, block);
        else blocks.push_back(block);
        for (auto it = vars.begin(); it != vars.end(); it++){
            if (it->first != "return")block->parentvars.emplace(it->first, &it->second);
        }
        if (parentvars.size() != 0){
            for (auto it = parentvars.begin(); it != parentvars.end(); it++){
                if (it->first != "return")block->parentvars.emplace(it->first, it->second);
            }
        }

        for (auto it = functions.begin(); it != functions.end(); it++){
            block->parentfunctions.emplace(it->first, it->second);
        }

        for (auto it = parentfunctions.begin(); it != parentfunctions.end(); it++){
            block->parentfunctions.emplace(it->first, it->second);
        }

        if (block->getType() == "loop"){
            loopblock = blocks.back();
            asloopblock = true;
        }
        current_block.current_block = block;
        current_block.nb_in_wait++;

        commandsorder.push_back(block);
        if (type == "mainblock" && !asloopblock){
            initOutLoopCommandsOrder.push_back(block);
        }
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
                current_block.current_block = blocks[i];
                break;
            }
        }
    }else{
        std::cout << "      ENDING me " << type << std::endl;
        setEnded();
    }

}

bool ScriptBlock::allBlocksEnded(){
    for (std::size_t i = 0; i < blocks.size(); i++){
        if (!blocks[i]->isEnded()){
            std::cout << "block " << blocks[i]->getType() << " not ended pos : " << i << std::endl;
            return false;
        }
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
    if (ptr != nullptr){
        return ptr->getLastEndedBlock();
    }
    return blocks.back();
}

bool ScriptBlock::functionExist(const std::string &name){
    auto ptrblock = current_block.current_block.lock();
    if (ptrblock != nullptr)return ptrblock->functionExist(name);
    return (functions.find(name)!=functions.end()) || (parentfunctions.find(name)!=parentfunctions.end());
}

std::weak_ptr<ScriptBlock> ScriptBlock::getFunction(const std::string &name){
    auto ptrblock = current_block.current_block.lock();
    if (ptrblock != nullptr)return ptrblock->getFunction(name);
    if (parentfunctions.find(name) != parentfunctions.end())return parentfunctions.at(name);
    if (functions.find(name) != functions.end())return functions.at(name);
}
