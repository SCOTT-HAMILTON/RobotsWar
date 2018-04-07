#include "ScriptReader.h"
#include <cstdlib>
#include <cmath>

std::vector<std::string> split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        if (end != start) {
          tokens.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    if (end != start) {
       tokens.push_back(text.substr(start));
    }
    return tokens;
}

ScriptReader::ScriptReader(const std::string &path_script)
{
    mainblock = std::make_shared<ScriptBlock>("mainblock");

    std::ifstream file_script;

    mainblock->setEnded();

    file_script.open(path_script);
    std::cout << "file : " << path_script << std::endl;
    if (file_script.good()){
        bool continuer = true;
        char buffer[1000];
        std::cout << "file good" << std::endl;
        while (continuer && !file_script.eof()){
            file_script.getline(buffer, sizeof(buffer));
            std::string line(buffer);
            if (line == "" || line == " " || line == "\n" || line == "\r\n"){
                std::cout << "line not good!" << std::endl;
            }
            std::cout << line << std::endl;
            std::shared_ptr<ScriptCommand> ptr;

            /* SYNTAXE TEST */

            if (line == "end"){
                mainblock->addBlockEnd();
            }
            else if (line.back() == ':'){
                std::cout << "line : " << line << ", new bloc !" << std::endl;
                mainblock->addBlock(line.substr(0, line.size()-1));
            }
            else if (line.rfind("move(", 0) == 0) {
                line = line.substr(5, line.size()-6);
                std::vector<std::string> params = split(line, ',');
                if (params.size()!=2){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                offset offsetx, offsety;
                char *success_x, *success_y;
                char *str_x = (char*)params[0].c_str(), *str_y = (char*)params[1].c_str();
                float x = std::strtof(str_x, &success_x);
                float y = std::strtof(str_y, &success_y);
                offsetx.floatval = x;offsety.floatval = y;
                offsetx.strval = params[0];offsety.strval = params[1];
                offsetx.type = CONSTANT;offsety.type = CONSTANT;
                if (std::isnan(x) || success_x == str_x){
                    offsetx.type = VAR;
                }//else std::cout << "const type x : "<<line<< std::endl;
                if (std::isnan(y) || success_y == str_y){
                    offsety.type = VAR;
                }//else std::cout << "const type y : "<<line<< std::endl;
                ptr = std::make_shared<MoveCommand>(offsetx, offsety);
                commands.push_back(ptr);

                mainblock->addCommand(std::weak_ptr<ScriptCommand>(commands.back()));

            }else if (line.rfind("float ", 0) == 0) {
                line = line.substr(6, line.size()-6);
                std::vector<std::string> tab = split(line, '=');
                if (tab.size()!=2){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                std::string name = tab[0];
                float val = 0;
                if (mainblock->evalParserExpr(tab[1], val)){
                    std::cout << "segmentation fault error during parsing trop lol: " << line << std::endl;
                    continuer = false;
                    break;
                }
                if (std::isnan(val)){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }

                mainblock->addVar(name, vartype(val));

            }else if (struct {std::vector<std::string> tab; std::size_t x;} v = {split(line, '='), v.tab.size()}; v.x == 2 ){
                if (!mainblock->varExist(v.tab[0])){
                    std::cout << "segmentation fault var : " << v.tab[0] << " not defined : " << line << std::endl;
                    continuer = false;
                    break;
                }

                float val = 0;
                if (mainblock->evalParserExpr(v.tab[1], val)){
                    std::cout << "segmentation fault error during parsing : " << line << std::endl;
                    continuer = false;
                    break;
                }
                if (std::isnan(val)){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                std::cout << "new value for " << v.tab[0] << " : " << val << std::endl;
                std::shared_ptr commandptr = std::make_shared<VarSetCommmand>(mainblock, v.tab[1], v.tab[0]);
                commands.push_back(commandptr);
                mainblock->addVar(v.tab[0], vartype(val));
                mainblock->addCommand(std::weak_ptr<ScriptCommand>(commands.back()));
            }
            /*-----SYNTAXE TEST END-----*/
        }

    }

    if (!mainblock->allBlocksEnded()){
        std::cout << "error during parsing all blocks are not ended !" << std::endl;
    }

    file_script.close();
}

ScriptReader::~ScriptReader()
{
    //dtor
}
void ScriptReader::getCommands(std::size_t start, std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands){
    return mainblock->getCommands(start, nbCommands, pCommands);
}

int ScriptReader::nbCommands(){
    return mainblock->nbCommands();
}

const vartype &ScriptReader::getVar(const std::string &var){
    return mainblock->getVar(var);
}

void ScriptReader::setVar(const std::string &var, float val){
    if (mainblock->varExist(var)){
        mainblock->addVar(var, vartype(val));
    }
}

void ScriptReader::displayMainBlockVars(){
    mainblock->displayVars();
}
