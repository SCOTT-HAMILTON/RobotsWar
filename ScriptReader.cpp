#include "ScriptReader.h"
#include <cstdlib>
#include <cmath>

const sf::Clock ScriptReader::UniversalRobotsTime = sf::Clock();


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

void ScriptReader::load(const std::string &path_script){
    std::ifstream file_script;
    file_script.open(path_script);
    std::cout << "file : " << path_script << std::endl;
    if (file_script.good()){
        bool continuer = true;
        char buffer[1000];
        std::cout << "file good" << std::endl;
        bool dontreadline = false;
        std::string touseline("");
        std::shared_ptr<ScriptCommand> toaddcommand;
        int linetoaddcommand = 0;
        unsigned long nbline = 0;
        while (continuer && !file_script.eof()){
            std::string line;
            nbline++;
            if (!dontreadline){
                file_script.getline(buffer, sizeof(buffer));
                line = std::string(buffer);
            }else line = touseline;
            line.erase(remove_if(line.begin(), line.end(), [](char c){return c == '\t' || c == ';';} ), line.end());
            std::cout << nbline << " LINE : " << line << std::endl;
            std::shared_ptr<ScriptCommand> ptr;
            bool entered = false;
            dontreadline = false;

            /* SYNTAXE TEST */

            if (line.rfind("end", 0) == 0){
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

                auto block = std::make_shared<FunctionBlock>(name, vars);
                mainblock->addBlock(block);
            }else if (line.rfind("move", 0) == 0) {
                entered = true;
                line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                line = line.substr(5, line.size()-6);
                std::vector<std::string> params = split(line, ',');
                if (params.size()!=2){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                ParamVar offsetx, offsety;
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

            }else if (line.rfind("turngmissileto", 0) == 0){
                entered = true;
                std::cout << "turn guided missile to !!" << std::endl;
                line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                line = line.substr(15, line.size()-16);
                std::vector<std::string> params = split(line, ',');
                if (params.size()!=2){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                ParamVar id, angle;
                char *success_id, *success_angle;
                char *str_id = (char*)params[0].c_str(), *str_angle = (char*)params[1].c_str();
                double val_id = std::strtof(str_id, &success_id);
                double val_angle = std::strtof(str_angle, &success_angle);
                id.doubleval = val_id;angle.doubleval = val_angle;
                id.expr = params[0];angle.expr = params[1];
                id.type = CONSTANT;angle.type = CONSTANT;

                if (std::isnan(val_id) || success_id == str_id){
                    id.type = VAR;
                }
                if (std::isnan(val_angle) || success_angle == str_angle){
                    angle.type = VAR;
                }
                std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();

                if (ptrblock.lock() != nullptr){
                    ptr = std::make_shared<GMissileTurnCommand>(ptrblock, id, angle);
                }else{
                    ptr = std::make_shared<GMissileTurnCommand>(mainblock, id, angle);
                }

                commands.push_back(ptr);

                mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptr));

            }else if (line.rfind("shootmissile", 0) == 0) {
                std::cout << "shoot misisle !!" << std::endl;
                entered = true;
                line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                line = line.substr(13, line.size()-14);
                std::cout << "line : " << line << std::endl;
                std::vector<std::string> params = split(line, ',');
                if (params.size()!=1){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                char *success;
                char *str = (char*)params[0].c_str();
                double x = std::strtof(str, &success);
                ParamVar offsetx;
                offsetx.doubleval = x;
                offsetx.expr = params[0];
                offsetx.type = CONSTANT;

                if (std::isnan(x) || success == str){
                    offsetx.type = VAR;
                }else std::cout << "param : " << offsetx.doubleval << std::endl;

                std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                if (ptrblock.lock() != nullptr){
                    ptr = std::make_shared<ShootMissileCommand>(ptrblock, offsetx);
                }else{
                    ptr = std::make_shared<ShootMissileCommand>(mainblock, offsetx);
                }

                commands.push_back(ptr);

                mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptr));

            }else if (line.rfind("shootguidedmissile", 0) == 0){
                std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                if (ptrblock.lock() != nullptr){
                    ptr = std::make_shared<ShootGuidedMissile>(ptrblock);
                }else{
                    ptr = std::make_shared<ShootGuidedMissile>(mainblock);
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
                }

                if (error){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }

                if (onexpr){
                    elems.push_back({str, EXPR});
                }else{
                    elems.push_back({str, STRING});
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
                replace_all(condition_expr, "==", "=");
                std::cout << "varname : " <<varname << ", condition_expr : " << condition_expr << ", startexpr : " << startexpr << ", incremeteexpr : " << incremeteexpr << std::endl;
                auto block = std::make_shared<ForBlock>(varname, condition_expr, startexpr, incremeteexpr);

                mainblock->addBlock(block);
                auto commandptr = std::make_shared<VarSetCommmand>(block, startexpr, varname);
                commands.push_back(commandptr);
                block->addNotPlayedCommand(commandptr);

                commandptr = std::make_shared<VarSetCommmand>(block, varname+"+"+incremeteexpr, varname);
                commands.push_back(commandptr);
                block->addNotPlayedCommand(commandptr);
            }else if (line.rfind("while", 0) == 0){
                entered = true;
                line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                std::cout << "new while !! line : " << line << std::endl;
                std::string condition_expr = line.substr(6, line.size()-7);
                replace_all(condition_expr, "and", "&");
                replace_all(condition_expr, "or", "&");
                replace_all(condition_expr, "==", "=");
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
                replace_all(condition_expr, "==", "=");
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
                replace_all(condition_expr, "==", "=");
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
                bool isInternalFunc = false;
                size_t index = tab[1].find_first_of('(');
                if (index != tab[1].npos){
                    size_t index_last = tab[1].find_last_of(')');
                    funcname = tab[1].substr(0, index);
                    touseline = tab[1];
                    isInternalFunc = std::find(internalFunctions.begin(), internalFunctions.end(), funcname) != internalFunctions.end();
                    if (!isInternalFunc){
                        std::cout << "funcname : " << funcname << std::endl;
                        std::string strvars = tab[1].substr(index+1, index_last-index-1);
                        std::cout << "strvars : " << strvars << std::endl;
                        std::vector<std::string> vars = split(strvars, ',');
                        std::cout << "first vars : " << vars[0] << std::endl;
                        if (mainblock->functionExist(funcname)){
                            addBlockCallFunc(funcname, vars);
                            func = true;
                        }
                    }else func = true;
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
                    std::shared_ptr<VarSetCommmand> commandptr;
                    if (!isInternalFunc){
                        std::shared_ptr<ScriptBlock> blockfuncptr = mainblock->getLastEndedBlock().lock();
                        std::shared_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock().lock();
                        if (ptrblock == nullptr){
                            ptrblock = mainblock;
                        }
                        double *val = ptrblock->getVarPtr(tab[0]);
                        blockfuncptr->addParentVar(tab[0], val);
                        commandptr = std::make_shared<VarSetCommmand>(ptrblock, "return", tab[0]);
                        commands.push_back(commandptr);
                        mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));

                    }else{
                        std::shared_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock().lock();
                        if (ptrblock == nullptr){
                            ptrblock = mainblock;
                        }
                        commandptr = std::make_shared<VarSetCommmand>(ptrblock, "return", tab[0]);
                        toaddcommand = commandptr;
                        linetoaddcommand = nbline+1;
                        dontreadline = true;
                    }

                    entered = true;
                }
            }if (line.rfind('=') != line.npos && !entered){

                line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                std::vector<std::string> tab = split(line, '=');
                if ( tab.size() != 2)break;

                std::string funcname("");
                double val = 0;
                size_t index = tab[1].find_first_of('(');
                bool isInternalFunc = false;
                std::shared_ptr<VarSetCommmand> commandptr;

                if (index != tab[1].npos){
                    size_t index_last = tab[1].find_last_of(')');
                    funcname = tab[1].substr(0, index);
                    touseline = tab[1];
                    if(std::find(internalFunctions.begin(), internalFunctions.end(), funcname) != internalFunctions.end()){
                        std::shared_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock().lock();
                        if (ptrblock == nullptr){
                            ptrblock = mainblock;
                        }
                        commandptr = std::make_shared<VarSetCommmand>(ptrblock, "return", tab[0]);
                        toaddcommand = commandptr;
                        linetoaddcommand = nbline+1;
                        dontreadline = true;
                    }else{
                        std::string strvars = tab[1].substr(index+1, index_last-index-1);
                        std::cout << "strvars : " << strvars << std::endl;
                        std::vector<std::string> vars = split(strvars, ',');
                        if (mainblock->functionExist(funcname)){
                            addBlockCallFunc(funcname, vars);
                        }
                        std::weak_ptr<ScriptBlock> blockfuncptr = mainblock->getLastEndedBlock();
                        std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                        if (ptrblock.lock() == nullptr){
                            ptrblock = mainblock;
                        }
                        double *val = ptrblock.lock()->getVarPtr(tab[0]);
                        blockfuncptr.lock()->addParentVar(tab[0], val);
                        commandptr = std::make_shared<VarSetCommmand>(ptrblock, "return", tab[0]);
                        commands.push_back(commandptr);
                        mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));
                    }
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
                    mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));
                }

            }
            if (toaddcommand != nullptr && linetoaddcommand == nbline){
                std::cout << "oui !!" << std::endl;
                commands.push_back(toaddcommand);
                mainblock->addCommand(std::weak_ptr<ScriptCommand>(toaddcommand));
                toaddcommand = nullptr;
            }
            /*-----SYNTAXE TEST END-----*/
        }

    }

    if (!mainblock->allBlocksEnded()){
        std::cout << "error during parsing all blocks are not ended !" << std::endl;
    }else{
        std::cout << "All blocks are correctly ended !! " << std::endl;
    }

    file_script.close();
    std::cout << std::endl << std::endl;
    std::cout << "FILE SCRIPT " << path_script << " PARSED !!!" << std::endl;
    std::cout << std::endl;
}

ScriptReader::ScriptReader()
{
    mainblock = std::make_shared<ScriptBlock>("mainblock");
    mainblock->setEnded();


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
        auto ptrblockcontainer = mainblock->getCurBlock().lock();
        if (ptrblockcontainer == nullptr)ptrblockcontainer = mainblock;
        std::cout << "ptr block container : " << ptrblockcontainer->getType() << std::endl;
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
                double *val = ptrblockcontainer->getPersonalVarPtr("return");
                funcblockptr->addParentVar("return" , val);

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

void ScriptReader::initVars(const sf::Vector2f &pos, const Map &worldmap, float dt){
    mainblock->addVar("time", UniversalRobotsTime.getElapsedTime().asMilliseconds());
    mainblock->addVar("posx", pos.x);mainblock->addVar("posy", pos.y);
    mainblock->addVar("dt", dt);
    sf::Vector2f middlepos(pos.x+(TILE_SIZE/2), pos.y+(TILE_SIZE/2));
    int tmpx = middlepos.x - (static_cast<int>(middlepos.x)%TILE_SIZE);tmpx/=TILE_SIZE;
    int tmpy = middlepos.y - (static_cast<int>(middlepos.y)%TILE_SIZE);tmpy/=TILE_SIZE;
    mainblock->addVar("mapx", tmpx);mainblock->addVar("mapy", tmpy);

    mainblock->addVar("blockupright", worldmap.getTile(tmpx+1, tmpy-1));
    mainblock->addVar("blockright", worldmap.getTile(tmpx+1, tmpy));
    mainblock->addVar("blockdownright", worldmap.getTile(tmpx+1, tmpy+1));
    mainblock->addVar("blockdown", worldmap.getTile(tmpx, tmpy+1));
    mainblock->addVar("blockdownleft", worldmap.getTile(tmpx-1, tmpy+1));
    mainblock->addVar("blockleft", worldmap.getTile(tmpx-1, tmpy));
    mainblock->addVar("blockupleft", worldmap.getTile(tmpx-1, tmpy-1));
    mainblock->addVar("blockup", worldmap.getTile(tmpx, tmpy-1));
}
