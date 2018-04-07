#include "ScriptBlock.h"

ScriptBlock::ScriptBlock(const std::string &type, bool ended) :
    asloopblock(false)
{
    this->type = type;
    this->ended = ended;
    parser = std::make_shared<Parser>();
    current_block.index = 0;
}

ScriptBlock::~ScriptBlock()
{
    //dtor
}

const vartype &ScriptBlock::getVar(const std::string &name){
    if (vars.find(name) == vars.end())return vartype(0);
    return vars[name];
}

const std::string &ScriptBlock::getString(const std::string &name){
    if (strings.find(name) == strings.end())return "";
    return strings[name];
}

void ScriptBlock::addVar(const std::string &name, const vartype &var){
    if (vars.find(name) != vars.end()){
        vars[name] = var;
    }else vars.insert(std::pair<std::string, vartype>(name, var));

}

void ScriptBlock::addString(const std::string &name, const std::string &str){
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
    }else{
        for (std::size_t i = start; i < nbCommands+start && i < commands.size(); i++){
            pCommands.push_back( std::weak_ptr<ScriptCommand>(commands[i]) );
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
    return commands.size();
}

void ScriptBlock::addCommand(const std::weak_ptr<ScriptCommand> &command){
    std::shared_ptr<ScriptBlock> block = current_block.current_block.lock();
    if (block != nullptr){
        block->addCommand(command);
        std::cout << "adding command " << command.lock()->getType() << " to " << block->getType() << std::endl;
    }
    else commands.push_back(command);
}

bool ScriptBlock::varExist(const std::string &name){
    return (vars.find(name) != vars.end()) || (strings.find(name) != strings.end());
}

int ScriptBlock::evalParserExpr(const std::string &expr, float &val){
    std::string varsstr("");
    std::size_t i = 0;
    for (auto it = vars.begin(); it != vars.end(); it++){
        varsstr += it->first;
        auto it2 = it;
        it2++;
        if (it2 != vars.end())varsstr += ", ";
        i++;
    }
    std::size_t err = parser->parser.parse(parser->bytecode, expr, varsstr);
    if (err){
        std::cout << parser->parser.errMessage(err) << std::endl;
        return 1;
    }
    i = 0;
    for (auto it = vars.begin(); it != vars.end(); it++){
        parser->bytecode.var[i] = std::get<float>(it->second);
        i++;
    }

    val = parser->bytecode.run();
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
    if (block == "loop"){
        loopblock = blocks.back();
        asloopblock = true;
    }
    current_block.current_block = blocks.back();
    current_block.index = blocks.size()-1;
    std::cout << "addblock : " << current_block.index << std::endl;
}

void ScriptBlock::addBlockEnd(){
    std::shared_ptr ptr = current_block.current_block.lock();
    if (ptr != nullptr){
        ptr->setEnded();
        if (current_block.index == 0) current_block.current_block = std::shared_ptr<ScriptBlock>();

        else {
            current_block.index--;
            current_block.current_block = blocks[current_block.index];

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
