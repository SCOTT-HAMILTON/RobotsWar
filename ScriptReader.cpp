#include "ScriptReader.h"
#include <cstdlib>
#include <cmath>

const sf::Clock ScriptReader::UniversalRobotsTime = sf::Clock();

//Parsing Tools

bool extractParams(std::string &line, size_t nb_params, std::vector<ParamVar> &params){
    line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
    size_t pos = line.find_first_of('(');
    if (pos == line.npos){
        std::cout << "error parsing function of line : " << line << '\n';
        return true;
    }
    line = line.substr(pos+1, line.size()-pos-2);
    std::cout << "line : " << line << '\n';
    std::vector<std::string> tab = split(line, ',');
    if (tab.size()!=nb_params){
        std::cout << "segmentation fault : " << line << '\n';
        return true;
    }
    for (size_t i = 0; i < nb_params; i++){
        ParamVar var;
        char *success_var;
        char *str_var = (char*)tab[i].c_str();
        double val_var = std::strtof(str_var, &success_var);
        var.doubleval = val_var;
        var.expr = tab[i];
        var.type = CONSTANT;
        if (std::isnan(val_var) || success_var == str_var){
            var.type = VAR;
        }
        params.push_back(var);
    }


    return false;
}

bool ScriptReader::parseConditionBlocks(bool &entered, std::string &line, bool &continuer){
    if (entered) return false;
    if (line.rfind("if", 0) == 0){
        entered = true;
        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
        std::cout << "new if !! line : " << line << '\n';
        std::string condition_expr = line.substr(3, line.size()-4);
        formateCondition(condition_expr);
        auto block = std::make_shared<IfBlock>(condition_expr);
        mainblock->addBlock(block);
        std::cout << "condition : " << condition_expr << '\n';
    }else if (line.rfind("else if", 0) == 0){
        entered = true;
        mainblock->addBlockEnd();
        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
        std::cout << "new else if !! line : " << line << '\n';
        std::string condition_expr = line.substr(7, line.size()-8);
        formateCondition(condition_expr);
        auto block = std::make_shared<ElseIfBlock>(condition_expr, mainblock->getLastEndedBlock());
        mainblock->addBlock(block);
        std::cout << "condition : " << condition_expr << '\n';
    }else if (line.rfind("else", 0) == 0){
        entered = true;
        mainblock->addBlockEnd();
        std::cout << "new else !! line : " << line << '\n';
        auto block = std::make_shared<ElseBlock>(mainblock->getLastEndedBlock());
        mainblock->addBlock(block);
    }
    return false;
}

bool ScriptReader::parseLoops(bool &entered, std::string &line, bool &continuer){
    if (entered) return false;
    if (line.rfind("for", 0) == 0){
        entered = true;
        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
        std::cout << "new for !! line : " << line << '\n';
        std::vector<std::string> tab = split(line.substr(4, line.size()-5), ',');
        std::string condition_expr;
        if (tab.size()!=3){
            std::cout << "segmentation fault : " << line << '\n';
            continuer = false;
        }
        condition_expr = tab[1];
        std::string tmp_var = tab[0];
        std::string incremeteexpr = tab[2];
        tab = split(tmp_var, '=');
        if (tab.size()!=2){
            std::cout << "segmentation fault : " << line << '\n';
            continuer = false;
        }
        std::string varname = tab[0];
        std::string startexpr = tab[1];
        formateCondition(condition_expr);
        std::cout << "varname : " <<varname << ", condition_expr : " << condition_expr << ", startexpr : " << startexpr << ", incremeteexpr : " << incremeteexpr << '\n';
        auto block = std::make_shared<ForBlock>(varname, condition_expr, startexpr, incremeteexpr);

        mainblock->addBlock(block);
        auto commandptr = std::make_shared<VarSetCommand>(block, startexpr, varname);
        commands.push_back(commandptr);
        block->addNotPlayedCommand(commandptr);

        commandptr = std::make_shared<VarSetCommand>(block, varname+"+"+incremeteexpr, varname);
        commands.push_back(commandptr);
        block->addNotPlayedCommand(commandptr);
    }else if (line.rfind("while", 0) == 0){
        entered = true;
        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
        std::cout << "new while !! line : " << line << '\n';
        std::string condition_expr = line.substr(6, line.size()-7);
        formateCondition(condition_expr);

        auto block = std::make_shared<WhileBlock>(condition_expr);
        mainblock->addBlock(block);
        block->me = block;
        std::cout << "condition : " << condition_expr << '\n';
    }
    return false;
}

bool ScriptReader::parseInternalFunctions(bool &entered, std::string &line, bool &continuer){
    if (entered) return false;
    if (line.rfind("getgmissileY", 0) == 0){
        entered = true;
        std::cout << "new get guided missile Y pos command !!\n";

        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
        line = line.substr(13, line.size()-14);
        std::vector<std::string> params = split(line, ',');
        if (params.size()!=1){
            std::cout << "segmentation fault : " << line << '\n';
            continuer = false;
        }
        ParamVar id;
        char *success_id;
        char *str_id = (char*)params[0].c_str();
        double val_id = std::strtof(str_id, &success_id);
        id.doubleval = val_id;
        id.expr = params[0];
        id.type = CONSTANT;
        if (std::isnan(val_id) || success_id == str_id){
            id.type = VAR;
        }

        std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();

        std::shared_ptr<ScriptCommand> ptrcommand;
        if (ptrblock.lock() != nullptr){
            ptrcommand = std::make_shared<getGMissileYCommand>(ptrblock, id);
        }else{
            ptrcommand = std::make_shared<getGMissileYCommand>(mainblock, id);
        }
        commands.push_back(ptrcommand);
        mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptrcommand));

    }
    else if (line.rfind("getgmissileX", 0) == 0){
        entered = true;
        std::cout << "new get guided missile X pos command !!\n";

        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
        line = line.substr(13, line.size()-14);
        std::vector<std::string> params = split(line, ',');
        if (params.size()!=1){
            std::cout << "segmentation fault : " << line << '\n';
            continuer = false;
        }
        ParamVar id;
        char *success_id;
        char *str_id = (char*)params[0].c_str();
        double val_id = std::strtof(str_id, &success_id);
        id.doubleval = val_id;
        id.expr = params[0];
        id.type = CONSTANT;
        if (std::isnan(val_id) || success_id == str_id){
            id.type = VAR;
        }

        std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();

        std::shared_ptr<ScriptCommand> ptrcommand;
        if (ptrblock.lock() != nullptr){
            ptrcommand = std::make_shared<getGMissileXCommand>(ptrblock, id);
        }else{
            ptrcommand = std::make_shared<getGMissileXCommand>(mainblock, id);
        }
        commands.push_back(ptrcommand);
        mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptrcommand));

    }
    else if (line.rfind("move", 0) == 0) {
        entered = true;
        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
        line = line.substr(5, line.size()-6);
        std::vector<std::string> params = split(line, ',');
        if (params.size()!=2){
            std::cout << "segmentation fault : " << line << '\n';
            continuer = false;
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
        }//else std::cout << "const type x : "<<line<< '\n';
        if (std::isnan(y) || success_y == str_y){
            offsety.type = VAR;
        }//else std::cout << "const type y : "<<line<< '\n';

        std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
        std::shared_ptr<ScriptCommand> ptrcommand;
        if (ptrblock.lock() != nullptr){
            ptrcommand = std::make_shared<MoveCommand>(ptrblock, offsetx, offsety);
        }else{
            ptrcommand = std::make_shared<MoveCommand>(mainblock, offsetx, offsety);
        }

        commands.push_back(ptrcommand);

        mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptrcommand));

    }else if (line.rfind("createblock", 0) == 0){
        entered = true;
        std::cout << "create block!!\n";
        std::vector<ParamVar> params;
        if (!extractParams(line, 2, params)){
            std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
            std::shared_ptr<ScriptCommand> ptrcommand;
            if (ptrblock.lock() != nullptr){
                ptrcommand = std::make_shared<CreateBlockCommand>(ptrblock, params[0], params[1]);
            }else{
                ptrcommand = std::make_shared<CreateBlockCommand>(mainblock, params[0], params[1]);
            }

            commands.push_back(ptrcommand);

            mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptrcommand));

        }else continuer = false;
    }else if (line.rfind("destroyblock", 0) == 0){
        entered = true;
        std::cout << "destroy block!!\n";
        std::vector<ParamVar> params;
        if (!extractParams(line, 1, params)){
            std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
            std::shared_ptr<ScriptCommand> ptrcommand;
            if (ptrblock.lock() != nullptr){
                ptrcommand = std::make_shared<DestroyBlockCommand>(ptrblock, params[0]);
            }else{
                ptrcommand = std::make_shared<DestroyBlockCommand>(mainblock, params[0]);
            }

            commands.push_back(ptrcommand);

            mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptrcommand));
        }else continuer = false;
    }else if (line.rfind("turngmissileto", 0) == 0){
        entered = true;
        std::cout << "turn guided missile to !!\n";
        std::vector<ParamVar> params;
        if (!extractParams(line, 2, params)){
            std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
            std::shared_ptr<ScriptCommand> ptrcommand;
            if (ptrblock.lock() != nullptr){
                ptrcommand = std::make_shared<GMissileTurnCommand>(ptrblock, params[0], params[1]);
            }else{
                ptrcommand = std::make_shared<GMissileTurnCommand>(mainblock, params[0], params[1]);
            }

            commands.push_back(ptrcommand);

            mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptrcommand));
        }else continuer = false;
    }else if (line.rfind("shootmissile", 0) == 0) {
        std::cout << "shoot misisle !!\n";
        entered = true;
        std::vector<ParamVar> params;
        if (!extractParams(line, 1, params)){
            std::shared_ptr<ScriptCommand> ptrcommand;
            std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
            if (ptrblock.lock() != nullptr){
                ptrcommand = std::make_shared<ShootMissileCommand>(ptrblock, params[0]);
            }else{
                ptrcommand = std::make_shared<ShootMissileCommand>(mainblock, params[0]);
            }

            commands.push_back(ptrcommand);
            mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptrcommand));
        }else continuer = false;
    }else if (line.rfind("shootguidedmissile", 0) == 0){
        std::cout << "new shoot guided missile command !!\n";
        std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
        std::shared_ptr<ScriptCommand> ptrcommand;
        if (ptrblock.lock() != nullptr){
            ptrcommand = std::make_shared<ShootGuidedMissile>(ptrblock);
        }else{
            ptrcommand = std::make_shared<ShootGuidedMissile>(mainblock);
        }
        commands.push_back(ptrcommand);
        mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptrcommand));
    }else if (line.rfind("print", 0) == 0) {
        entered = true;
        std::size_t tmpindex = line.find_first_of('(')+1;
        line = line.substr(tmpindex, line.find_last_of(')')-tmpindex);
        std::cout << "new print command !! line : " << line  << '\n';
        std::vector<PrintElem> elems;
        bool onstr = false, onexpr = false, strfinished = true;
        std::string str("");
        bool error = false;
        for (std::size_t i = 0; i < line.size(); i++){
            if (line[i] == '+'){
                if (!onstr && !onexpr){
                    error = true;
                }
                if (onexpr){
                    elems.push_back({str, EXPR});
                }else{
                    elems.push_back({str, STRING});
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
            std::cout << "segmentation fault : " << line << '\n';
            continuer = false;
        }

        if (onexpr){
            elems.push_back({str, EXPR});
        }else{
            elems.push_back({str, STRING});
        }

        std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
        std::shared_ptr<ScriptCommand> ptrcommand;
        if (ptrblock.lock() != nullptr){
            ptrcommand = std::make_shared<PrintCommand>(ptrblock, elems);
        }else{
            ptrcommand = std::make_shared<PrintCommand>(mainblock, elems);
        }

        commands.push_back(ptrcommand);

        mainblock->addCommand(std::weak_ptr<ScriptCommand>(ptrcommand));

    }
    return false;
}

void ScriptReader::createInternalFuncVarSet(const std::string &varname, const std::string &funcstr, size_t current_line){
    std::shared_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock().lock();
    if (ptrblock == nullptr){
        ptrblock = mainblock;
    }
    std::shared_ptr<ScriptCommand> commandptr = std::make_shared<VarSetCommand>(ptrblock, "return", varname);
    toaddcommand = commandptr;
    linetoaddcommand = current_line+1;
    touseline = funcstr;
}

void ScriptReader::createFuncVarSet(const std::string &varname, const std::string &funcname, const std::vector<std::string> &vars){
    addBlockCallFunc(funcname, vars);
    std::shared_ptr<ScriptBlock> blockfuncptr = mainblock->getLastEndedBlock().lock();
    std::shared_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock().lock();
    if (ptrblock == nullptr){
        ptrblock = mainblock;
    }
    double *val = ptrblock->getVarPtr(varname);//
    blockfuncptr->addParentVar(varname, val);  //ink return of func to current scope return*/
    std::shared_ptr<ScriptCommand> commandptr = std::make_shared<VarSetCommand>(ptrblock, "return", varname);
    commands.push_back(commandptr);
    mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));
}

bool ScriptReader::functionExist(const std::string &funcname){
    return internalFunctionExist(funcname)
    ||
    mainblock->functionExist(funcname);
}

bool internalFunctionExist(const std::string &funcname){
    return std::find(internalFunctions.begin(), internalFunctions.end(), funcname) != internalFunctions.end();
}

void formateCondition(std::string &condition_expr){
    replace_all(condition_expr, "and", "&");
    replace_all(condition_expr, "or", "|");
    replace_all(condition_expr, "==", "=");
}

void parseFuncStr(const std::string &str, std::string &name, std::vector<std::string> &vars){
    std::string line = str;
    line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
    size_t index_first = line.find_first_of('(');
    if (index_first != line.npos){
        size_t index_last = line.find_last_of(')');
        name = line.substr(0, index_first);
        std::string strvars = line.substr(index_first+1, index_last-index_first-1);
        vars = split(strvars, ',');
    }
}


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
    if (tokens.size() == 1 && tokens[0] == "")tokens.clear();
    return tokens;
}

//End Tools

void ScriptReader::load(const std::string &path_script){
    std::ifstream file_script;
    file_script.open(path_script);
    bool continuer = true;
    std::cout << "file : " << path_script << '\n';
    if (file_script.good()){
        char buffer[1000];
        std::cout << "file good\n";
        unsigned long nbline = 0;
        while (continuer && !file_script.eof()){
            std::string line;
            nbline++;
            if (touseline == ""){
                file_script.getline(buffer, sizeof(buffer));
                line = std::string(buffer);
            }else line = touseline;
            touseline = "";
            line.erase(remove_if(line.begin(), line.end(), [](char c){return c == '\t' || c == ';';} ), line.end());
            std::cout << nbline << " LINE : " << line << '\n';

            bool entered = false;
            /* SYNTAXE TESTS */

            //Internal Functions
            parseInternalFunctions(entered, line, continuer);
            if (!entered && continuer){
                //Base Syntaxe
                parseLoops(entered, line, continuer);
                parseConditionBlocks(entered, line, continuer);
                if (!entered && continuer){
                    if (line.rfind("end", 0) == 0){
                        entered = true;
                        mainblock->addBlockEnd();
                    }else if (line.rfind("//", 0) == 0){
                        entered = true;
                        std::cout << "com!\n";
                    }else if (line.rfind("func", 0) == 0){
                        entered = true;
                        std::vector<std::string> vars;
                        std::string name;
                        line = line.substr(4, line.size()-5);
                        parseFuncStr(line, name, vars);
                        std::cout << "body definition function \"" << name << "\"\n";
                        std::cout << "vars : \n";
                        for (auto &it : vars){
                            std::cout << "it : " << it << "\n";
                        }
                        std::cout << "creating !!!\n";
                        std::cout << "vars size func definition : " << vars.size() << "\n";
                        auto block = std::make_shared<FunctionBlock>(name, vars);
                        if (block == nullptr) std::cout << "nullptr func block !!\n";
                        std::cout << "finished !!\n";
                        mainblock->addFunctionBlock(block);
                        std::cout << "created !!\n";
                    }else if (line.back() == ':'){
                        entered = true;
                        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                        mainblock->addBlock(line);
                    }else if (line.rfind("return", 0) == 0){
                        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                        std::cout << "return command !! " << line << '\n';
                        std::string expr = line.substr(6, line.size()-6);
                        std::cout << "expr : " << expr << '\n';
                        std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                        std::shared_ptr<VarSetCommand> commandptr;
                        if (ptrblock.lock() != nullptr){
                            commandptr = std::make_shared<VarSetCommand>(ptrblock, expr, "return");
                        }else{
                            commandptr = std::make_shared<VarSetCommand>(mainblock, expr, "return");
                        }

                        commands.push_back(commandptr);
                        mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));
                    }else if (std::string funcname = line.substr(0, line.find_first_of('(')); mainblock->functionExist(funcname) ){
                        std::vector<std::string> vars;
                        parseFuncStr(line, funcname, vars);
                        addBlockCallFunc(funcname, vars);
                    }else if (line.rfind("double ", 0) == 0) {
                        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                        line = line.substr(6, line.size()-6);
                        std::vector<std::string> tab = split(line, '=');
                        if (tab.size()!=2){
                            std::cout << "segmentation fault : " << line << '\n';
                            continuer = false;
                            break;
                        }

                        std::string varname = tab[0];
                        std::string funcstr = tab[1];
                        std::string expr = tab[1];
                        std::vector<std::string> vars;
                        std::string funcname("");
                        parseFuncStr(funcstr, funcname, vars);
                        if (functionExist(funcname)){
                            if (!internalFunctionExist(funcname)){
                                createFuncVarSet(varname, funcname, vars);
                            }else{
                                createInternalFuncVarSet(varname, funcstr, nbline);
                            }

                            entered = true;
                        }
                        mainblock->addVar(varname, -1);
                    }if (line.rfind('=') != line.npos && !entered){
                        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                        std::vector<std::string> tab = split(line, '=');
                        if ( tab.size() != 2)break;

                        std::string varname = tab[0];
                        std::string funcstr = tab[1];
                        std::string expr = tab[1];
                        std::vector<std::string> vars;
                        std::string funcname("");
                        parseFuncStr(funcstr, funcname, vars);

                        if (functionExist(funcname)){
                            if(internalFunctionExist(funcname)){
                                createInternalFuncVarSet(varname, funcstr, nbline);
                            }else if (mainblock->functionExist(funcname)){
                                createFuncVarSet(varname, funcname, vars);
                            }
                        }

                        else {

                            if (!mainblock->varExist(varname)){
                                std::cout << "segmentation fault var " << varname << " not defined : " << line << '\n';
                                continuer = false;
                                break;
                            }
                            double val;
                            if (mainblock->evalParserExpr(expr, val)){
                                std::cout << "segmentation fault error during parsing : " << expr << '\n';
                                continuer = false;
                                break;
                            }
                            if (std::isnan(val)){
                                std::cout << "segmentation fault string evaluated doesn't give a number : " << line << '\n';
                                continuer = false;
                                break;
                            }
                            std::weak_ptr<ScriptBlock> ptrblock = mainblock->getCurBlock();
                            std::shared_ptr<VarSetCommand> commandptr;
                            if (ptrblock.lock() != nullptr){
                                commandptr = std::make_shared<VarSetCommand>(ptrblock, expr, varname);
                            }else{
                                commandptr = std::make_shared<VarSetCommand>(mainblock, expr, varname);
                            }

                            commands.push_back(commandptr);
                            mainblock->addCommand(std::weak_ptr<ScriptCommand>(commandptr));
                        }

                    }
                }
            }


            if (continuer && toaddcommand != nullptr && linetoaddcommand == nbline){
                commands.push_back(toaddcommand);
                mainblock->addCommand(std::weak_ptr<ScriptCommand>(toaddcommand));
                toaddcommand = nullptr;
            }
            /*-----SYNTAXE TEST END-----*/
        }
    }

    if (!continuer){
        std::cout << "error during parsing !!\n";
    }
    else if (!mainblock->allBlocksEnded()){
        std::cout << "error during parsing all blocks are not ended !\n";
    }else{
        std::cout << "All blocks are correctly ended !! \n";
    }

    file_script.close();
    std::cout << '\n' << '\n';
    std::cout << "FILE SCRIPT " << path_script << " PARSED !!!\n";
    std::cout << '\n';
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
        std::cout << "ptr block container : " << ptrblockcontainer->getType() << '\n';
        mainblock->addBlock("call"+funcname+"func");
        auto funcblockptr = mainblock->getCurBlock().lock();
        if (funcblockptr != nullptr){
            auto funcptr = mainblock->getFunction(funcname).lock();
            if (funcptr != nullptr){
                std::size_t index = 0;
                //PARAMETERS DROPPING
                funcptr->copyVarsTo(funcblockptr);
                funcptr->copyBlocksTo(funcblockptr);
                funcptr->copyCommandsOrderTo(funcblockptr);
                for (auto &it : exprs){
                    std::string expr = std::string(it);
                    std::string varname = funcptr->getVarName(index);
                    std::cout << "adding var " << varname << " to " << funcblockptr->getType() << " equal to " << expr << '\n';
                    funcblockptr->addVar(varname, 0);
                    commands.push_back(std::make_shared<VarSetCommand>(std::weak_ptr<ScriptBlock>(funcblockptr), expr, varname));
                    funcblockptr->addCommand(commands.back());
                    commands.back()->setBlock(funcblockptr);
                    index++;
                }
                double *val = ptrblockcontainer->getPersonalVarPtr("return");
                funcblockptr->addParentVar("return" , val);
            }else std::cout << "error script reader addblockcallfunc, funcblock is nullptr\n";
        }else std::cout << "error script reader addblockcallfunc, block is nullptr\n";
        mainblock->addBlockEnd();
    }else{
        std::cout << "error script reader addblockcallfunc, funcname doesn't exist: " << funcname << '\n';
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
