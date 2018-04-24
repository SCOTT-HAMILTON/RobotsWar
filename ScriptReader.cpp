#include "ScriptReader.h"
#include <cstdlib>
#include <cmath>

void replace_all( std::string &s, const std::string &tosearch, const std::string &toreplace ) {
    for( size_t pos = 0; ; pos += toreplace.length() ) {
        pos = s.find( tosearch, pos );
        if( pos == std::string::npos ) break;
        s.erase( pos, tosearch.length() );
        s.insert( pos, toreplace );
    }
}

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
            line.erase(remove_if(line.begin(), line.end(), [](char c){return c == '\t' || c == ';';} ), line.end());
            std::cout << "LINE : " << line << std::endl;
            std::shared_ptr<ScriptCommand> ptr;

            /* SYNTAXE TEST */

            if (line == "end"){
                mainblock->addBlockEnd();
            }
            else if (line.rfind("move(", 0) == 0) {
                line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
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
                double x = std::strtof(str_x, &success_x);
                double y = std::strtof(str_y, &success_y);
                offsetx.doubleval = x;offsety.doubleval = y;
                offsetx.expr = params[0];offsety.expr = params[1];
                offsetx.type = CONSTANT;offsety.type = CONSTANT;
                if (std::isnan(x) || success_x == str_x){
                    offsetx.type = VAR;
                }//else std::cout << "const type x : "<<line<< std::endl;
                if (std::isnan(y) || success_y == str_y){
                    offsety.type = VAR;
                }//else std::cout << "const type y : "<<line<< std::endl;

                std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                if (ptrblock.lock() != nullptr){
                    ptr = std::make_shared<MoveCommand>(ptrblock, offsetx, offsety);
                }else{
                    ptr = std::make_shared<MoveCommand>(mainblock, offsetx, offsety);
                }

                commands.push_back(ptr);

                mainblock->addCommand(std::weak_ptr<ScriptCommand>(commands.back()));

            }else if (line.rfind("double ", 0) == 0) {
                line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                line = line.substr(6, line.size()-6);
                std::vector<std::string> tab = split(line, '=');
                if (tab.size()!=2){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                std::string name = tab[0];
                double val = 0;
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
                mainblock->addVar(name, val);
            }if (line.rfind('=') != line.npos){
                line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                std::vector<std::string> tab = split(line, '=');
                if ( tab.size() != 2)break;

                if (!mainblock->varExist(tab[0])){
                    std::cout << "segmentation fault var " << tab[0] << " not defined : " << line << std::endl;
                    continuer = false;
                    break;
                }

                double val = 0;
                if (mainblock->evalParserExpr(tab[1], val)){
                    std::cout << "segmentation fault error during parsing : " << line << std::endl;
                    continuer = false;
                    break;
                }
                if (std::isnan(val)){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                std::cout << "new value for " << tab[0] << " : " << val << std::endl;
                std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                std::shared_ptr<VarSetCommmand> commandptr;
                if (ptrblock.lock() != nullptr){
                    commandptr = std::make_shared<VarSetCommmand>(ptrblock, tab[1], tab[0]);
                }else{
                    commandptr = std::make_shared<VarSetCommmand>(mainblock, tab[1], tab[0]);
                }


                commands.push_back(commandptr);
                mainblock->addVar(tab[0], val);
                mainblock->addCommand(std::weak_ptr<ScriptCommand>(commands.back()));
            }else if (line.rfind("if", 0) == 0){
                line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                std::cout << "new if !! line : " << line << std::endl;
                std::string condition_expr = line.substr(3, line.size()-4);
                replace_all(condition_expr, "and", "&");
                replace_all(condition_expr, "or", "&");
                auto block = std::make_shared<IfBlock>(condition_expr);
                mainblock->addBlock(block);
                std::cout << "condition : " << condition_expr << std::endl;
            }else if (line.rfind("else if", 0) == 0){
                mainblock->addBlockEnd();
                line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                std::cout << "new else if !! line : " << line << std::endl;
                std::string condition_expr = line.substr(7, line.size()-8);
                replace_all(condition_expr, "and", "&");
                replace_all(condition_expr, "or", "&");
                auto block = std::make_shared<ElseIfBlock>(condition_expr, mainblock->getLastEndedBlock());
                mainblock->addBlock(block);
                std::cout << "condition : " << condition_expr << std::endl;
            }else if (line.rfind("else", 0) == 0){
                mainblock->addBlockEnd();
                std::cout << "new else !! line : " << line << std::endl;
                auto block = std::make_shared<ElseBlock>(mainblock->getLastEndedBlock());
                mainblock->addBlock(block);
            }else if (line.back() == ':'){
                mainblock->addBlock(line.substr(0, line.size()-1));
            }
            /*-----SYNTAXE TEST END-----*/
        }

    }

    if (!mainblock->allBlocksEnded()){
        std::cout << "error during parsing all blocks are not ended !" << std::endl;
    }

    file_script.close();
    std::cout << std::endl << std::endl;
    std::cout << "FILE SCRIPT " << path_script << " PARSED !!!" << std::endl;
    std::cout << std::endl;
}

ScriptReader::~ScriptReader()
{
    //dtor
}
void ScriptReader::getCommands(std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands){
    mainblock->getCommands(nbCommands, pCommands);
}

int ScriptReader::nbCommands(){
    return mainblock->nbCommands();
}

double ScriptReader::getVar(const std::string &var){
    return mainblock->getVar(var);
}

void ScriptReader::setVar(const std::string &var, double val){
    if (mainblock->varExist(var)){
        mainblock->addVar(var, val);
    }
}

void ScriptReader::displayMainBlockVars(){
    mainblock->displayVars();
}
