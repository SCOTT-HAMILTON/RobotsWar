#include "Blocks/ScriptBlock.h"
#include "Commands/BlockEntryCommand.h"
#include <cstring>
#include <cstdlib>
#include <iterator>
#include <list>

ScriptBlock::ScriptBlock(const std::string &_type, bool _ended) :
    type(_type), ended(_ended), asloopblock(false), index_lastcmd(0), execInitCommands(false), last_could_enter(true),
    add_blockentry_test_on_getcmds(false)
{

    std::cerr << "\n\n\t Script block " << type <<  " created \n\n\n";

    name = type;
    //this->parentvars = parentvars;
    current_block.nb_in_wait = 0;
    current_block.current_block = nullptr;
    debug_times = 0;

    if (type.rfind("call", 0) != 0)addVar("return", 0);
    else std::cerr << "function cc no personal return !!\n";
}

ScriptBlock::~ScriptBlock()
{

}

double ScriptBlock::getVar(const std::string &_name){
    if (current_block.current_block != nullptr)return current_block.current_block->getVar(_name);
    if (parentvars.find(_name) != parentvars.end())return *parentvars.at(_name);
    if (vars.find(_name) != vars.end())return vars.at(_name);

    return double(0);
}

double *ScriptBlock::getVarPtr(const std::string &_name){
    if (current_block.current_block != nullptr)return current_block.current_block->getVarPtr(_name);
    if (parentvars.find(_name) != parentvars.end())return parentvars.at(_name);
    if (vars.find(_name) != vars.end())return &vars.at(_name);

    return nullptr;
}

double *ScriptBlock::getPersonalVarPtr(const std::string &_name){
    if (parentvars.find(_name) != parentvars.end())return parentvars.at(_name);
    if (vars.find(_name) != vars.end())return &vars.at(_name);

    return nullptr;
}

std::string ScriptBlock::getVarName(size_t index){
    if (current_block.current_block != nullptr)return current_block.current_block->getVarName(index);
    size_t i = 0;
    if (index>=vars.size()){
        std::cerr << "error index too big !! index : " << index << ", varssize : " << vars.size() << ", type : " << type << '\n';
    }else {
        for (auto it = vars.begin(); it != vars.end(); it++){
            if (i == index)return it->first;
            i++;
        }

    }
    return "";
}

const std::string &ScriptBlock::getString(const std::string &_name){
    if (current_block.current_block != nullptr)return current_block.current_block->getString(_name);
    if (strings.find(_name) != strings.end())return strings[_name];
    return _name;
}

void ScriptBlock::addVar(const std::string &_name, double var){
    if (current_block.current_block != nullptr){
        current_block.current_block->addVar(_name, var);
    }else{
        if (vars.find(_name) != vars.end()){
            vars[_name] = var;
        }else if (parentvars.find(_name) != parentvars.end()){
            *parentvars[_name] = var;
        }else{
            vars.emplace(_name, var);
        }
    }
}

void ScriptBlock::addParentVar(const std::string &_name, double *var){
    if (current_block.current_block != nullptr){
        current_block.current_block->addParentVar(_name, var);
    }else{
        parentvars[_name] = var;
        std::cerr << "new parentvar block " << type << " : " << _name << " val " << *var << '\n';
    }
}

void ScriptBlock::addString(const std::string &_name, const std::string &str){
    if (current_block.current_block != nullptr)return current_block.current_block->addString(_name, str);
    if (strings.find(_name) != strings.end()){
        strings[_name] = str;
    }else strings.insert(std::pair<std::string, std::string>(_name, str));
}

bool ScriptBlock::getCommands(size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands, bool &commandsended){
    commandsended = false;
    size_t commands_done = 0;
    //tempcommands.clear();
    for (int i = static_cast<int>(tempcommands.size()-1); i >= 0; i--){
        if (static_cast<bool>(tempcommands[static_cast<size_t>(i)]->getProp("used")))tempcommands.erase(tempcommands.begin()+i);
        else{
            size_t nb_loop_unused = static_cast<size_t>(tempcommands[static_cast<size_t>(i)]->getProp("nb_loop_unused")+1);
            tempcommands[static_cast<size_t>(i)]->setProp("nb_loop_unused", nb_loop_unused);
            if (nb_loop_unused>5){
                std::cerr << "error command still not used : " << tempcommands[static_cast<size_t>(i)]->getType() << "\n";
                tempcommands.erase(tempcommands.begin()+i);
            }
        }
    }
    bool launched = false;
    std::vector<std::variant<std::reference_wrapper<ScriptBlock>, std::reference_wrapper<ScriptCommand>>> *commands_to_drop = &commandsorder;

    if (!launched){

        if (index_lastcmd>=commands_to_drop->size())index_lastcmd = 0;
        bool last_ended_properly = true;
        for (size_t i = index_lastcmd; i < commands_to_drop->size() && commands_done<nbCommands && last_ended_properly; i++){
            index_lastcmd = i;
            last_ended_properly = true;
            try{
                ScriptBlock &block = std::get<std::reference_wrapper<ScriptBlock>>(commands_to_drop->at(i)).get();
                if (block.getType() == "functionblock");
                else{

                    //std::cerr << "get commands of " << block.type << "\n";
                    /*std::cerr << "cmds before block get commands : \n";
                    for (auto &cmd : pCommands){
                        if (cmd.get().getType() == "varset")std::cerr << "varset cmd " << cmd.get().getStringProp("varname") << " = " << cmd.get().getStringProp("expr") << "\n";
                        else std::cerr << cmd.get().getType() << " cmd\n";
                    }std::cerr << "\n";
                    */
                    if (!block.add_blockentry_test_on_getcmds){
                        auto tmp_blockentry = std::make_unique<BlockEntryCommand>(block);
                        tempcommands.push_back( std::move(tmp_blockentry) );
                        tempcommands.back()->setProp("nb_loop_unused", 0);
                        tempcommands.back()->setProp("used", 0);
                        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*tempcommands.back().get());
                        pCommands.push_back( ref_cmd );
                        size_t before = pCommands.size();
                        block.getCommands(nbCommands-commands_done, pCommands, last_ended_properly);
                        commands_done += pCommands.size()-before;

                        if (tempcommands.back()->getProp("nbcmd") == 0.0){
                            static_cast<BlockEntryCommand*>(tempcommands.back().get())->setNbCmd(pCommands.size()-before);
                        }
                    }
                    else{
                        size_t before = pCommands.size();
                        block.getCommands(nbCommands-commands_done, pCommands, last_ended_properly);
                        commands_done += pCommands.size()-before;
                    }
                    /*std::cerr << "cmds after " << block.getType() << " get commands : \n";
                    for (auto &cmd : pCommands){
                        if (cmd.get().getType() == "varset")std::cerr << "varset cmd " << cmd.get().getStringProp("varname") << " = " << cmd.get().getStringProp("expr") << "\n";
                        else std::cerr << cmd.get().getType() << " cmd\n";
                    }std::cerr << "\n";
                    */
                }
            }
            catch (const std::bad_variant_access&) {
                auto cmd = std::get<std::reference_wrapper<ScriptCommand>>(commands_to_drop->at(i));
                pCommands.push_back(cmd);
                commands_done++;
            }
            index_lastcmd = i;
        }
        if (last_ended_properly){
            index_lastcmd++;

        }
        if (index_lastcmd>=commands_to_drop->size()){
            index_lastcmd = 0;
            execInitCommands = true;
            commandsended = true;
        }
    }
    /*for (auto &c : pCommands){
        auto &cmd = c.get();
        if (cmd.getType() == "varset") std::cerr << "cmd " << cmd.getStringProp("varname") << " = " << cmd.getStringProp("expr") << '\n';
        else std::cerr << "cmd " << cmd.getType() << '\n';
    }*/
    return commandsended;
}

void ScriptBlock::addCommand(std::reference_wrapper<ScriptCommand> command){
    if (current_block.current_block != nullptr){
        std::cerr << "              NEW COMMAND " << command.get().getType() << " to " << current_block.current_block->getType() << '\n';
        current_block.current_block->addCommand(command);
    }
    else {
        if (command.get().getType() == "varset")std::cerr << "              NEW VARSET COMMAND " << command.get().getStringProp("varname") << "=" << command.get().getStringProp("expr") << " to me " << type << '\n';
        else std::cerr << "              NEW COMMAND " << command.get().getType() << " to me " << type << '\n';
        commandsorder.push_back(command);
    }
}

void ScriptBlock::addNotPlayedCommand(std::reference_wrapper<ScriptCommand> command){
    if (current_block.current_block != nullptr){
        current_block.current_block->addNotPlayedCommand(command);
    }else {
        commands.push_back(command);
    }
}

void ScriptBlock::copyVarsTo(ScriptBlock &block){
    block.vars = vars;
    block.parentvars = parentvars;
}

void ScriptBlock::copyBlocksTo(ScriptBlock &block){
    block.blocks = blocks;
}
void ScriptBlock::copyCommandsOrderTo(ScriptBlock &block){
    block.commandsorder.clear();/*
    for (auto &c : commandsorder){
        try{
            ScriptBlock &block = std::get<std::reference_wrapper<ScriptBlock>>(c).get();
            if (block.getType().rfind("call", 0) == 0 ){

            }
        }
    }*/
}

bool ScriptBlock::varExist(const std::string &_name){

    if (current_block.current_block != nullptr){
        return current_block.current_block->varExist(_name);
    }

    return (vars.find(_name) != vars.end() ||
            strings.find(_name) != strings.end() ||
            parentvars.find(_name) != parentvars.end());
}

int ScriptBlock::evalParserExpr(const std::string &expr, double &val){
    if (current_block.current_block != nullptr)return current_block.current_block->evalParserExpr(expr, val);

    if (varExist(expr)){
        val = getVar(expr);
        return 0;
    }
    try{
        val = std::stod(expr);
        return 0;
    }catch(std::exception &){
    }
    std::string varsstr("");
    size_t i = 0;
    const size_t sizevarstr = vars.size()+parentvars.size();
    double tmpvars[sizevarstr];
    for (auto it = vars.begin(); it != vars.end(); it++){
        std::string var(it->first);
        var.erase(remove_if(var.begin(), var.end(), [](char c)
        {
            return std::isspace(c);
        }
        ), var.end());

        varsstr += var;
        auto it2 = it;
        it2++;
        if (it2 != vars.end())varsstr += ",";
        tmpvars[i] = it->second;
        i++;
    }

    if (parentvars.size() != 0){
        if (varsstr != "")varsstr += ",";
        for (auto it = parentvars.begin(); it != parentvars.end(); it++){
            std::string var(it->first);
            var.erase(remove_if(var.begin(), var.end(), [](char c)
            {
                return std::isspace(c);
            }
            ), var.end());
            varsstr += var;
            auto it2 = it;
            it2++;
            if (it2 != parentvars.end())varsstr += ",";
            tmpvars[i] = *it->second;
            i++;
        }
    }
    int code;
    Parser *ptrparser = nullptr;
    if (exprevaluated.find(expr+varsstr) != exprevaluated.end()){
        ptrparser = exprevaluated[expr+varsstr].get();
        //std::cerr << "expr with those vars already parsed.\n";
        if (ptrparser == nullptr)std::cerr << "ptr parser is nullptr!!\n";
    }
    else {
        auto newparser = std::make_unique<Parser>();
        code = newparser->Parse(expr, varsstr);
        if (code >= 0){
            std::cerr << "error parsing expr \"" << expr << "\"\n";
            std::cerr << "me : " << type << ", vars : " << varsstr << '\n';
            std::cerr << "error msg : " << ptrparser->ErrorMsg() << '\n';
            return 1;
        }
        ptrparser = newparser.get();
        exprevaluated.emplace(expr+varsstr, std::move(newparser));
        //ptrparser->Optimize();
    }

    val = ptrparser->Eval(tmpvars);
    code = ptrparser->EvalError();
    if (code > 0){
        std::cerr << "error evaluanting expr " << expr << "error code : " << code <<  '\n';
        return 1;
    }
    return 0;
}

void ScriptBlock::setEnded(){
    ended = true;
}

void ScriptBlock::setLastCouldEnter(bool couldenter)
{
    last_could_enter = couldenter;
}

bool ScriptBlock::isEnded(){
    return ended;
}

void ScriptBlock::addBlock(const std::string &blockstr){
    if (current_block.current_block != nullptr){
        current_block.current_block->addBlock(blockstr);
    }else{
        std::cerr << "ADDING lexical block " << blockstr << " to me " << type << '\n';
        auto tmpblock = std::make_unique<ScriptBlock>(blockstr);
        std::reference_wrapper<ScriptBlock> ref_block = std::ref(*tmpblock);
        blocks.push_back( ref_block );
        ownedblocks.push_back( std::move(tmpblock) );
        for (auto it = vars.begin(); it != vars.end(); it++){
            if (it->first != "return")blocks.back().get().parentvars.emplace(it->first, &it->second);
        }
        if (parentvars.size() != 0){
            for (auto it = parentvars.begin(); it != parentvars.end(); it++){
                if (it->first != "return")blocks.back().get().parentvars.emplace(it->first, it->second);
            }
        }

        for (auto it = functions.begin(); it != functions.end(); it++){
            blocks.back().get().parentfunctions.emplace(it->first, it->second);
        }

        for (auto it = parentfunctions.begin(); it != parentfunctions.end(); it++){
            blocks.back().get().parentfunctions.emplace(it->first, it->second);
        }


        if (blockstr == std::string("loop")){
            loopblock = &blocks.back().get();
            asloopblock = true;
        }
        current_block.current_block = &blocks.back().get();
        current_block.nb_in_wait++;

        commandsorder.push_back(blocks.back());

        //vars test

        std::string varsstr("");
        for (auto it = blocks.back().get().vars.begin(); it != blocks.back().get().vars.end(); it++){
            varsstr += it->first;
            auto it2 = it;
            it2++;
            if (it2 != blocks.back().get().vars.end())varsstr += ", ";
        }
        std::cerr << "VARS : " << varsstr << '\n';

        varsstr = "";

        if (blocks.back().get().parentvars.size() != 0){
            if (varsstr != "")varsstr += ", ";
            for (auto it = blocks.back().get().parentvars.begin(); it != blocks.back().get().parentvars.end(); it++){
                varsstr += it->first;
                auto it2 = it;
                it2++;
                if (it2 != blocks.back().get().parentvars.end())varsstr += ", ";
            }
        }
        std::cerr << "PARENTVARS : " << varsstr << '\n';

    }
}

void ScriptBlock::addBlock(std::reference_wrapper<ScriptBlock> block){
    if (current_block.current_block != nullptr){
        current_block.current_block->addBlock(block);
    }else{
        blocks.push_back(block);
        if (vars.size() != 0){
            for (auto it = vars.begin(); it != vars.end(); it++){
                if (it->first != "return")block.get().parentvars.emplace(it->first, &it->second);
            }
        }
        if (parentvars.size() != 0){
            for (auto it = parentvars.begin(); it != parentvars.end(); it++){
                if (it->first != "return")block.get().parentvars.emplace(it->first, it->second);
            }
        }
        for (auto it = functions.begin(); it != functions.end(); it++){
            block.get().parentfunctions.emplace(it->first, it->second);
        }
        for (auto it = parentfunctions.begin(); it != parentfunctions.end(); it++){
            block.get().parentfunctions.emplace(it->first, it->second);
        }

        if (block.get().getType() == "loop"){
            loopblock = &blocks.back().get();
            asloopblock = true;
        }
        current_block.current_block = &block.get();
        current_block.nb_in_wait++;
        commandsorder.push_back(block);
        //vars test

        std::string varsstr("");
        if (blocks.size() != 0){
            for (auto it = blocks.back().get().vars.begin(); it != blocks.back().get().vars.end(); it++){
                varsstr += it->first;
                auto it2 = it;
                it2++;
                if (it2 != blocks.back().get().vars.end())varsstr += ", ";
            }
            std::cerr << "VARS : " << varsstr << '\n';
            varsstr = "";

            if (blocks.back().get().parentvars.size() != 0){
                if (varsstr != "")varsstr += ", ";
                for (auto it = blocks.back().get().parentvars.begin(); it != blocks.back().get().parentvars.end(); it++){
                    varsstr += it->first;
                    auto it2 = it;
                    it2++;
                    if (it2 != blocks.back().get().parentvars.end())varsstr += ", ";
                }
            }
            std::cerr << "PARENTVARS : " << varsstr << '\n';
        }
    }
}

void ScriptBlock::addBlockEnd(){
    if (current_block.current_block != nullptr){
        current_block.current_block->addBlockEnd();
        if (current_block.current_block->isEnded())current_block.nb_in_wait--;
        current_block.current_block = nullptr;
        for (auto it = functions.begin(); it != functions.end(); it++){
            if (!it->second.get().isEnded()){
                current_block.current_block = &it->second.get();
            }
        }
        for (int i = static_cast<int>(blocks.size()-1); i >= 0; i--){
            if (!blocks[static_cast<size_t>(i)].get().isEnded()){
                current_block.current_block = &blocks[static_cast<size_t>(i)].get();
                break;
            }
        }
    }else{
        std::cerr << "      ENDING me " << type << '\n';
        setEnded();
    }

}

bool ScriptBlock::allBlocksEnded(){
    for (size_t i = 0; i < blocks.size(); i++){
        if (!blocks[i].get().isEnded()){
            std::cerr << "block " << blocks[i].get().getType() << " not ended pos : " << i << '\n';
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

const std::string &ScriptBlock::getName()
{
    return name;
}

ScriptBlock *ScriptBlock::getCurBlock(){
    if (current_block.current_block != nullptr){
        auto tmpptr = current_block.current_block->getCurBlock();
        if (tmpptr == nullptr){
            return current_block.current_block;
        }
        return tmpptr;
    }
    return current_block.current_block;
}

bool ScriptBlock::getStat(){
    return stat;
}

bool ScriptBlock::isCondChainEntered(){
    return condchain_entered;
}


std::reference_wrapper<ScriptBlock> ScriptBlock::getLastEndedBlock(){
    if (current_block.current_block != nullptr){
        return current_block.current_block->getLastEndedBlock();
    }
    return blocks.back();
}

bool ScriptBlock::functionExist(const std::string &_name){
    if (current_block.current_block != nullptr)return current_block.current_block->functionExist(_name);
    return (functions.find(_name)!=functions.end()) || (parentfunctions.find(_name)!=parentfunctions.end());
}

std::reference_wrapper<ScriptBlock> ScriptBlock::getFunction(const std::string &_name){
    if (current_block.current_block != nullptr)return current_block.current_block->getFunction(_name);
    if (parentfunctions.find(_name) != parentfunctions.end())return parentfunctions.at(_name);
    if (functions.find(_name) != functions.end())return functions.at(_name);
    auto tmp = std::make_unique<ScriptBlock>(std::string("null"));
    std::reference_wrapper<ScriptBlock> ref_block = std::ref( *tmp );
    return ref_block;
}

bool ScriptBlock::getResultLastCanEnter(){
    return resultLastCanEnter;
}

bool ScriptBlock::isAddingBlockentryTestOnGetcmds()
{
    return add_blockentry_test_on_getcmds;
}

void ScriptBlock::addMeParentFunction(const std::string &funcname, std::reference_wrapper<ScriptBlock> func)
{
    parentfunctions.emplace(funcname, func);
}

void ScriptBlock::addMeVar(const std::string &varname, double val)
{
    vars.emplace(varname, val);
}

void ScriptBlock::addMeParentVar(const std::string &varname, double *val)
{
    parentvars.emplace(varname, val);
}
