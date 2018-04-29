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
            bool entered = false;

            /* SYNTAXE TEST */

            if (line == "end"){
                entered = true;
                mainblock->addBlockEnd();
            }else if (line.rfind("//", 0) == 0){
                entered = true;
                std::cout << "com!" << std::endl;
            }else if (line.rfind("func", 0) == 0){
                entered = true;
                line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                size_t index_first = line.find_first_of('(');
                std::string name = line.substr(4, index_first-4);
                std::cout << "new function : " << name << std::endl;
                std::vector<std::string> vars;
                size_t index_last = line.find_last_of(')');
                std::string strvars = line.substr(index_first+1, index_last-index_first-1);
                std::cout << "strvars : " << strvars << std::endl;
                vars = split(strvars, ',');
                std::cout << "first vars : " << vars[0] << std::endl;

                auto block = std::make_shared<FunctionBlock>(name, vars);
                mainblock->addBlock(block);
            }
            else if (line.rfind("move", 0) == 0) {
                entered = true;
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

                mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptr));

            }else if (line.rfind("print", 0) == 0) {
                entered = true;
                std::size_t tmpindex = line.find_first_of('(')+1;
                line = line.substr(tmpindex, line.find_last_of(')')-tmpindex);
                std::cout << "new print command !! line : " << line  << std::endl;
                std::vector<PrintElem> elems;
                bool onstr = false, onexpr = false, strfinished = true;
                std::string str("");
                bool error = false;
                for (std::size_t i = 0; i < line.size(); i++){
                    if (line[i] == '+'){
                        if (!onstr && !onexpr){
                            error = true;
                            break;
                        }
                        if (onexpr){
                            elems.push_back({str, EXPR});
                            std::cout << "expression for print : " << str << std::endl;
                        }else{
                            elems.push_back({str, STRING});
                            std::cout << "string for print : " << str << std::endl;
                        }
                        onexpr = false;
                        strfinished = true;
                        onstr = false;
                        str = "";
                    }
                    else if (onstr){
                        if (line[i] != '"' && !strfinished )str += line[i];
                        else if (line[i] == '"'){
                            if (str.size()>1 && str.back() == '\\'){
                                str.pop_back();
                                str += '"';
                            }else{
                                strfinished = true;
                            }
                        }
                    }else if (onexpr){
                        if (!isspace(line[i]))str += line[i];
                    }
                    else if (line[i] == '"'){
                        str = "";
                        onstr = true;
                        strfinished = false;
                    }else if (!std::isspace(line[i])){
                        str = "";
                        str += line[i];
                        onexpr = true;
                    }
                    std::cout << "str : " << str << ", " << line[i] << std::endl;
                }

                if (error){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }

                if (onexpr){
                    elems.push_back({str, EXPR});
                    std::cout << "expression for (out loop) print : " << str << std::endl;
                }else{
                    elems.push_back({str, STRING});
                    std::cout << "string for (out loop) print : " << str << std::endl;
                }

                std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                if (ptrblock.lock() != nullptr){
                    ptr = std::make_shared<PrintCommand>(ptrblock, elems);
                }else{
                    ptr = std::make_shared<PrintCommand>(mainblock, elems);
                }

                commands.push_back(ptr);

                mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptr));

            }else if (line.rfind("for", 0) == 0){
                entered = true;
                line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                std::cout << "new for !! line : " << line << std::endl;
                std::vector<std::string> tab = split(line.substr(4, line.size()-5), ',');
                std::string condition_expr;
                if (tab.size()!=3){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                condition_expr = tab[1];
                std::string tmp_var = tab[0];
                std::string incremeteexpr = tab[2];
                tab = split(tmp_var, '=');
                if (tab.size()!=2){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                std::string varname = tab[0];
                std::string startexpr = tab[1];
                replace_all(condition_expr, "and", "&");
                replace_all(condition_expr, "or", "&");
                auto block = std::make_shared<ForBlock>(varname, condition_expr, startexpr, incremeteexpr);
                mainblock->addBlock(block);
                std::cout << "condition : " << condition_expr << ", varname : " << varname << ", startexpr " << startexpr << ", incremente expr ! " << incremeteexpr << std::endl;
            }else if (line.rfind("while", 0) == 0){
                entered = true;
                line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                std::cout << "new while !! line : " << line << std::endl;
                std::string condition_expr = line.substr(6, line.size()-7);
                replace_all(condition_expr, "and", "&");
                replace_all(condition_expr, "or", "&");
                auto block = std::make_shared<WhileBlock>(condition_expr);
                mainblock->addBlock(block);
                std::cout << "condition : " << condition_expr << std::endl;
            }else if (line.rfind("if", 0) == 0){
                entered = true;
                line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                std::cout << "new if !! line : " << line << std::endl;
                std::string condition_expr = line.substr(3, line.size()-4);
                replace_all(condition_expr, "and", "&");
                replace_all(condition_expr, "or", "&");
                auto block = std::make_shared<IfBlock>(condition_expr);
                mainblock->addBlock(block);
                std::cout << "condition : " << condition_expr << std::endl;
            }else if (line.rfind("else if", 0) == 0){
                entered = true;
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
                entered = true;
                mainblock->addBlockEnd();
                std::cout << "new else !! line : " << line << std::endl;
                auto block = std::make_shared<ElseBlock>(mainblock->getLastEndedBlock());
                mainblock->addBlock(block);
            }else if (line.back() == ':'){
                entered = true;
                line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                mainblock->addBlock(line);
            }else if (line.rfind("return", 0) == 0){
                line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                std::cout << "return command !! " << line << std::endl;
                std::string expr = line.substr(6, line.size()-6);
                std::cout << "expr : " << expr << std::endl;
                std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                std::shared_ptr<VarSetCommmand> commandptr;
                if (ptrblock.lock() != nullptr){
                    commandptr = std::make_shared<VarSetCommmand>(ptrblock, expr, "return");
                }else{
                    commandptr = std::make_shared<VarSetCommmand>(mainblock, expr, "return");
                }

                commands.push_back(commandptr);
                mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));
            }else if (std::string name = line.substr(0, line.find_first_of('(')); mainblock->functionExist(name) ){
                std::cout << "function call !! !! " << std::endl;
                size_t index = line.find_first_of('(');
                if (index != line.npos){
                    size_t index_last = line.find_last_of(')');
                    std::string funcname = line.substr(0, index);
                    std::cout << "funcname : " << funcname << std::endl;
                    std::string strvars = line.substr(index+1, index_last-index-1);
                    std::cout << "strvars : " << strvars << std::endl;
                    std::vector<std::string> vars = split(strvars, ',');
                    std::cout << "first vars : " << vars[0] << std::endl;
                    if (mainblock->functionExist(funcname)){
                        addBlockCallFunc(funcname, vars);
                    }
                }
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
                std::string funcname("");
                double val = 0;
                bool func = false;
                size_t index = tab[1].find_first_of('(');
                if (index != tab[1].npos){
                    size_t index_last = tab[1].find_last_of(')');
                    funcname = tab[1].substr(0, index);
                    std::cout << "funcname : " << funcname << std::endl;
                    std::string strvars = tab[1].substr(index+1, index_last-index-1);
                    std::cout << "strvars : " << strvars << std::endl;
                    std::vector<std::string> vars = split(strvars, ',');
                    std::cout << "first vars : " << vars[0] << std::endl;
                    if (mainblock->functionExist(funcname)){
                        addBlockCallFunc(funcname, vars);
                        func = true;
                    }
                }
                if (!func){
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
                }
                mainblock->addVar(name, val);

                if (func){
                    std::shared_ptr<ScriptBlock> blockfuncptr = mainblock->getLastEndedBlock().lock();
                    std::shared_ptr<VarSetCommmand> commandptr;
                    std::shared_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock().lock();
                    if (ptrblock == nullptr){
                        ptrblock = mainblock;
                    }
                    double *val = ptrblock->getVarPtr(tab[0]);
                    std::cout << "adding parent var " << tab[0] << " of val " << *val << " and of block " << ptrblock->getType() << " to func block " << blockfuncptr->getType() << std::endl;
                    blockfuncptr->addParentVar(tab[0], val);
                    commandptr = std::make_shared<VarSetCommmand>(blockfuncptr, "return", tab[0]);
                    commands.push_back(commandptr);
                    mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));
                    entered = true;
                }
            }if (line.rfind('=') != line.npos && !entered){
                line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                std::vector<std::string> tab = split(line, '=');
                if ( tab.size() != 2)break;

                std::string funcname("");
                double val = 0;
                size_t index = tab[1].find_first_of('(');
                if (index != tab[1].npos){
                    size_t index_last = tab[1].find_last_of(')');
                    funcname = tab[1].substr(0, index);
                    std::cout << "funcname : " << funcname << std::endl;
                    std::string strvars = tab[1].substr(index+1, index_last-index-1);
                    std::cout << "strvars : " << strvars << std::endl;
                    std::vector<std::string> vars = split(strvars, ',');
                    std::cout << "first vars : " << vars[0] << std::endl;
                    if (mainblock->functionExist(funcname)){
                        addBlockCallFunc(funcname, vars);
                    }
                    std::weak_ptr<ScriptBlock> blockfuncptr = mainblock->getLastEndedBlock();
                    std::shared_ptr<VarSetCommmand> commandptr;
                    std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                    if (ptrblock.lock() == nullptr){
                        ptrblock = mainblock;
                    }
                    double *val = ptrblock.lock()->getVarPtr(tab[0]);
                    blockfuncptr.lock()->addParentVar(tab[0], val);
                    if (blockfuncptr.lock() != nullptr){
                        std::cout << "setting " << tab[0] << " to return of block " << blockfuncptr.lock()->getType() << std::endl;
                        commandptr = std::make_shared<VarSetCommmand>(blockfuncptr, "return", tab[0]);
                    }else{
                        commandptr = std::make_shared<VarSetCommmand>(mainblock, "return", tab[0]);
                    }

                    commands.push_back(commandptr);
                    mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));
                }

                else {
                    if (!mainblock->varExist(tab[0])){
                        std::cout << "segmentation fault var " << tab[0] << " not defined : " << line << std::endl;
                        continuer = false;
                        break;
                    }
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
                    std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                    std::shared_ptr<VarSetCommmand> commandptr;
                    if (ptrblock.lock() != nullptr){
                        commandptr = std::make_shared<VarSetCommmand>(ptrblock, tab[1], tab[0]);
                    }else{
                        commandptr = std::make_shared<VarSetCommmand>(mainblock, tab[1], tab[0]);
                    }

                    commands.push_back(commandptr);

                    mainblock->addVar(tab[0], val);
                    mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));
                }
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
    bool tmp;
    mainblock->getCommands(nbCommands, pCommands, tmp);
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

void ScriptReader::addBlockCallFunc(const std::string &funcname, const std::vector<std::string> &exprs){
    if (mainblock->functionExist(funcname)){
        mainblock->addBlock("call"+funcname+"func");
        auto funcblockptr = mainblock->getCurBlock().lock();
        if (funcblockptr != nullptr){
            auto funcptr = mainblock->getFunction(funcname).lock();
            if (funcptr != nullptr){
                std::size_t index = 0;
                //PARAMETERS DROPPING
                for (auto it : exprs){
                    std::string expr = std::string(it);
                    std::string varname = funcptr->getVarName(index);
                    std::cout << "adding var " << varname << " to " << funcblockptr->getType() << " equal to " << expr << std::endl;
                    funcblockptr->addVar(varname, 0);
                    commands.push_back(std::make_shared<VarSetCommmand>(std::weak_ptr<ScriptBlock>(funcblockptr), expr, varname));
                    funcblockptr->addCommand(commands.back());
                    commands.back()->setBlock(funcblockptr);
                    index++;
                }

                std::vector<std::weak_ptr<ScriptCommand>> cmds;
                bool tmpbool;
                funcptr->getCommands(funcptr->nbCommands(), cmds, tmpbool);
                for (const auto &it : cmds){
                    it.lock()->setBlock(funcblockptr);
                    std::cout << "adding command " << it.lock()->getType() << ", to block " << funcblockptr->getType() << std::endl;
                    funcblockptr->addCommand(it);
                }

            }else std::cout << "error script reader addblockcallfunc, funcblock is nullptr" << std::endl;
        }else std::cout << "error script reader addblockcallfunc, block is nullptr" << std::endl;
        mainblock->addBlockEnd();
    }else{
        std::cout << "error script reader addblockcallfunc, funcname doesn't exist: " << funcname << std::endl;
    }
}
