#include "ScriptReader.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

const sf::Clock ScriptReader::UniversalRobotsTime = sf::Clock();

//Parsing Tools



std::string remove_ws( const std::string& str )
{
    std::string str_no_ws ;
    for( char c : str ) if( !std::isspace(c) ) str_no_ws += c ;
    return str_no_ws;
}

ScriptReader::ScriptReader()
{
    auto tmp = std::make_unique<MainBlock>();
    mainblock = std::move(tmp);
    mainblock->setEnded();
}

bool extractParams(std::string &line, size_t nb_params, std::vector<ParamVar> &params){
    line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
    size_t pos = line.find_first_of('(');
    if (pos == line.npos){
        std::cerr << "error parsing function of line : " << line << '\n';
        return true;
    }
    line = line.substr(pos+1, line.size()-pos-2);
    std::cerr << "line : " << line << '\n';
    std::vector<std::string> tab = split(line, ',');
    if (tab.size()!=nb_params){
        std::cerr << "segmentation fault : " << line << '\n';
        return true;
    }
    for (size_t i = 0; i < nb_params; i++){
        ParamVar var;
        char *success_var;
        const char *str_var = tab[i].c_str();
        double val_var = std::strtod(str_var, &success_var);
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

bool ScriptReader::parseConditionBlocks(bool &entered, std::string &line){
    if (entered) return false;
    if (line.rfind("if", 0) == 0){
        entered = true;
        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
        std::cerr << "new if !! line : " << line << '\n';
        std::string condition_expr = line.substr(3, line.size()-4);
        formateCondition(condition_expr);
        std::unique_ptr<ScriptBlock> block = std::make_unique<IfBlock>(condition_expr);
        std::reference_wrapper<ScriptBlock> ref_block = std::ref(*block.get());
        mainblock->addBlock( ref_block );
        blocks.push_back(std::move(block));
        std::cerr << "condition : " << condition_expr << '\n';
    }else if (line.rfind("else if", 0) == 0){
        entered = true;
        mainblock->addBlockEnd();
        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
        std::cerr << "new else if !! line : " << line << '\n';
        std::string condition_expr = line.substr(7, line.size()-8);
        formateCondition(condition_expr);
        ScriptBlock &ref_lastendedblock = mainblock->getLastEndedBlock().get();
        std::unique_ptr<ScriptBlock> block = std::make_unique<ElseIfBlock>(condition_expr, ref_lastendedblock);
        std::reference_wrapper<ScriptBlock> ref_block = std::ref(*block.get());
        mainblock->addBlock( ref_block );
        blocks.push_back(std::move(block));
        std::cerr << "condition : " << condition_expr << '\n';
    }else if (line.rfind("else", 0) == 0){
        entered = true;
        mainblock->addBlockEnd();
        std::cerr << "new else !! line : " << line << '\n';
        ScriptBlock &ref_lastendedblock = mainblock->getLastEndedBlock().get();
        std::unique_ptr<ScriptBlock> block = std::make_unique<ElseBlock>(ref_lastendedblock);
        std::reference_wrapper<ScriptBlock> ref_block = std::ref(*block.get());
        mainblock->addBlock( ref_block );
        blocks.push_back(std::move(block));
    }
    return false;
}

bool ScriptReader::parseLoops(bool &entered, std::string &line, bool &continuer){
    if (entered) return false;
    if (line.rfind("for", 0) == 0){
        entered = true;
        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
        std::cerr << "new for !! line : " << line << '\n';
        std::vector<std::string> tab = split(line.substr(4, line.size()-5), ',');
        std::string condition_expr;
        if (tab.size()!=3){
            std::cerr << "segmentation fault : " << line << '\n';
            continuer = false;
        }
        condition_expr = tab[1];
        std::string tmp_var = tab[0];
        std::string incremeteexpr = tab[2];
        tab = split(tmp_var, '=');
        if (tab.size()!=2){
            std::cerr << "segmentation fault : " << line << '\n';
            continuer = false;
        }
        std::string varname = tab[0];
        std::string startexpr = tab[1];
        formateCondition(condition_expr);
        std::cerr << "varname : " <<varname << ", condition_expr : " << condition_expr << ", startexpr : " << startexpr << ", incremeteexpr : " << incremeteexpr << '\n';
        std::unique_ptr<ScriptBlock> block = std::make_unique<ForBlock>(varname, condition_expr, startexpr, incremeteexpr);
        ScriptBlock &block_ref = *block;
        {
            std::unique_ptr<ScriptCommand> command_varset_startexpr = std::make_unique<VarSetCommand>(block_ref, startexpr, varname);
            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*command_varset_startexpr.get());
            block->addNotPlayedCommand( ref_cmd );
            commands.emplace_back(std::move(command_varset_startexpr));
        }

        {
            std::unique_ptr<ScriptCommand> command_varset_incremente_expr = std::make_unique<VarSetCommand>(block_ref, varname+"+"+incremeteexpr, varname);
            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*command_varset_incremente_expr.get());
            block->addNotPlayedCommand( ref_cmd );
            commands.emplace_back(std::move(command_varset_incremente_expr));
        }

        {
            std::unique_ptr<ScriptCommand> command_blockentry = std::make_unique<BlockEntryCommand>(block_ref);
            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*command_blockentry.get());
            block->addNotPlayedCommand( ref_cmd );
            commands.emplace_back(std::move(command_blockentry));
        }

        std::reference_wrapper<ScriptBlock> ref_block = std::ref(*block.get());
        mainblock->addBlock( ref_block );
        blocks.push_back(std::move(block));
    }else if (line.rfind("while", 0) == 0){
        entered = true;
        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
        std::cerr << "new while !! line : " << line << '\n';
        std::string condition_expr = line.substr(6, line.size()-7);
        formateCondition(condition_expr);

        std::unique_ptr<ScriptBlock> block = std::make_unique<WhileBlock>(condition_expr);
        ScriptBlock &block_ref = *block;
        {
            std::unique_ptr<ScriptCommand> command_blockentry = std::make_unique<BlockEntryCommand>(block_ref);
            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*command_blockentry.get());
            block->addNotPlayedCommand(ref_cmd);
            commands.emplace_back(std::move(command_blockentry));
        }
        std::reference_wrapper<ScriptBlock> ref_block = std::ref(block_ref);
        mainblock->addBlock( ref_block );
        blocks.push_back(std::move(block));
        std::cerr << "condition : " << condition_expr << '\n';
    }
    return false;
}

bool ScriptReader::parseInternalFunctions(bool &entered, std::string &line, bool &continuer){
    if (entered) return false;
    if (line.rfind("getgmissileY", 0) == 0){
        entered = true;
        std::cerr << "new get guided missile Y pos command !!\n";

        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
        line = line.substr(13, line.size()-14);
        std::vector<std::string> params = split(line, ',');
        if (params.size()!=1){
            std::cerr << "segmentation fault : " << line << '\n';
            continuer = false;
        }
        ParamVar id;
        char *success_id;
        const char *str_id = params[0].c_str();
        double val_id = std::strtod(str_id, &success_id);
        id.doubleval = val_id;
        id.expr = params[0];
        id.type = CONSTANT;
        if (std::isnan(val_id) || success_id == str_id){
            id.type = VAR;
        }

        ScriptBlock *ptrblock = mainblock->getCurBlock();

        std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
        if (ptrblock != nullptr){
            ScriptBlock &ref_block = *ptrblock;
            ptrcommand = std::make_unique<getGMissileYCommand>(ref_block, id);
        }else{
            ScriptBlock &ref_block = *mainblock;
            ptrcommand = std::make_unique<getGMissileYCommand>(ref_block, id);
        }
        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
        mainblock->addCommand(ref_cmd);
        commands.emplace_back(std::move(ptrcommand));
    }
    else if (line.rfind("getgmissileX", 0) == 0){
        entered = true;
        std::cerr << "new get guided missile X pos command !!\n";

        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
        line = line.substr(13, line.size()-14);
        std::vector<std::string> params = split(line, ',');
        if (params.size()!=1){
            std::cerr << "segmentation fault : " << line << '\n';
            continuer = false;
        }
        ParamVar id;
        char *success_id;
        const char *str_id = params[0].c_str();
        double val_id = std::strtod(str_id, &success_id);
        id.doubleval = val_id;
        id.expr = params[0];
        id.type = CONSTANT;
        if (std::isnan(val_id) || success_id == str_id){
            id.type = VAR;
        }

        ScriptBlock *ptrblock = mainblock->getCurBlock();

        std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
        if (ptrblock != nullptr){
            ptrcommand = std::make_unique<getGMissileXCommand>(*ptrblock, id);
        }else{
            ptrcommand = std::make_unique<getGMissileXCommand>(*mainblock, id);
        }
        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
        mainblock->addCommand(ref_cmd);
        commands.emplace_back(std::move(ptrcommand));
    }
    else if (line.rfind("move", 0) == 0) {
        entered = true;
        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
        line = line.substr(5, line.size()-6);
        std::vector<std::string> params = split(line, ',');
        if (params.size()!=2){
            std::cerr << "segmentation fault : " << line << '\n';
            continuer = false;
        }
        ParamVar offsetx, offsety;
        char *success_x, *success_y;
        const char *str_x = params[0].c_str(), *str_y = params[1].c_str();
        double x = std::strtod(str_x, &success_x);
        double y = std::strtod(str_y, &success_y);
        offsetx.doubleval = x;offsety.doubleval = y;
        offsetx.expr = params[0];offsety.expr = params[1];
        offsetx.type = CONSTANT;offsety.type = CONSTANT;

        if (std::isnan(x) || success_x == str_x){
            offsetx.type = VAR;
        }//else std::cerr << "const type x : "<<line<< '\n';
        if (std::isnan(y) || success_y == str_y){
            offsety.type = VAR;
        }//else std::cerr << "const type y : "<<line<< '\n';

        ScriptBlock *ptrblock = mainblock->getCurBlock();

        std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
        if (ptrblock != nullptr){
            ptrcommand = std::make_unique<MoveCommand>(*ptrblock, offsetx, offsety);
        }else{
            ptrcommand = std::make_unique<MoveCommand>(*mainblock, offsetx, offsety);
        }
        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
        mainblock->addCommand(ref_cmd);
        commands.emplace_back(std::move(ptrcommand));

    }else if (line.rfind("createblock", 0) == 0){
        entered = true;
        std::cerr << "create block!!\n";
        std::vector<ParamVar> params;
        if (!extractParams(line, 2, params)){
            ScriptBlock *ptrblock = mainblock->getCurBlock();

            std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
            if (ptrblock != nullptr){
                ptrcommand = std::make_unique<CreateBlockCommand>(*ptrblock, params[0], params[1]);
            }else{
                ptrcommand = std::make_unique<CreateBlockCommand>(*mainblock, params[0], params[1]);
            }
            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
            mainblock->addCommand(ref_cmd);
            commands.emplace_back(std::move(ptrcommand));

        }else continuer = false;
    }else if (line.rfind("destroyblock", 0) == 0){
        entered = true;
        std::cerr << "destroy block!!\n";
        std::vector<ParamVar> params;
        if (!extractParams(line, 1, params)){
            ScriptBlock *ptrblock = mainblock->getCurBlock();

            std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
            if (ptrblock != nullptr){
                ptrcommand = std::make_unique<DestroyBlockCommand>(*ptrblock, params[0]);
            }else{
                ptrcommand = std::make_unique<DestroyBlockCommand>(*mainblock, params[0]);
            }
            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
            mainblock->addCommand(ref_cmd);
            commands.emplace_back(std::move(ptrcommand));
        }else continuer = false;
    }else if (line.rfind("turngmissileto", 0) == 0){
        entered = true;
        std::cerr << "turn guided missile to !!\n";
        std::vector<ParamVar> params;
        if (!extractParams(line, 2, params)){
            ScriptBlock *ptrblock = mainblock->getCurBlock();

            std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
            if (ptrblock != nullptr){
                ptrcommand = std::make_unique<GMissileTurnCommand>(*ptrblock, params[0], params[1]);
            }else{
                ptrcommand = std::make_unique<GMissileTurnCommand>(*mainblock, params[0], params[1]);
            }
            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
            mainblock->addCommand(ref_cmd);
            commands.emplace_back(std::move(ptrcommand));
        }else continuer = false;
    }else if (line.rfind("shootmissile", 0) == 0) {
        std::cerr << "shoot misisle !!\n";
        entered = true;
        std::vector<ParamVar> params;
        if (!extractParams(line, 1, params)){
            ScriptBlock *ptrblock = mainblock->getCurBlock();

            std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
            if (ptrblock != nullptr){
                ptrcommand = std::make_unique<ShootMissileCommand>(*ptrblock, params[0]);
            }else{
                ptrcommand = std::make_unique<ShootMissileCommand>(*mainblock, params[0]);
            }
            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
            mainblock->addCommand(ref_cmd);
            commands.emplace_back(std::move(ptrcommand));
        }else continuer = false;
    }else if (line.rfind("shootguidedmissile", 0) == 0){
        std::cerr << "new shoot guided missile command !!\n";
        ScriptBlock *ptrblock = mainblock->getCurBlock();

        std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
        if (ptrblock != nullptr){
            ptrcommand = std::make_unique<ShootGuidedMissileCommand>(*ptrblock);
        }else{
            ptrcommand = std::make_unique<ShootGuidedMissileCommand>(*mainblock);
        }
        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
        mainblock->addCommand(ref_cmd);
        commands.emplace_back(std::move(ptrcommand));
    }else if (line.rfind("print", 0) == 0) {
        entered = true;
        std::size_t tmpindex = line.find_first_of('(')+1;
        line = line.substr(tmpindex, line.find_last_of(')')-tmpindex);
        std::cerr << "new print command !! line : " << line  << '\n';
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
            std::cerr << "segmentation fault : " << line << '\n';
            continuer = false;
        }

        if (onexpr){
            elems.push_back({str, EXPR});
        }else{
            elems.push_back({str, STRING});
        }
        ScriptBlock *ptrblock = mainblock->getCurBlock();

        std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
        if (ptrblock != nullptr){
            ptrcommand = std::make_unique<PrintCommand>(*ptrblock, elems);
        }else{
            ptrcommand = std::make_unique<PrintCommand>(*mainblock, elems);
        }
        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
        mainblock->addCommand(ref_cmd);
        commands.emplace_back(std::move(ptrcommand));

    }
    return false;
}

void ScriptReader::createInternalFuncVarSet(const std::string &varname, const std::string &funcstr, size_t current_line){
    ScriptBlock *ptrblock = mainblock->getCurBlock();
    if (ptrblock == nullptr){
        ptrblock = mainblock.get();
    }
    std::unique_ptr<ScriptCommand> commandptr = std::make_unique<VarSetCommand>(*ptrblock, "return", varname);
    toaddcommand = std::move(commandptr);
    linetoaddcommand = current_line+1;
    touseline = funcstr;
}

void ScriptReader::createFuncVarSet(const std::string &varname, const std::string &funcname, const std::vector<std::string> &vars){
    addBlockCallFunc(funcname, vars);
    ScriptBlock &blockfuncptr = mainblock->getLastEndedBlock().get();
    ScriptBlock *ptrblock = mainblock->getCurBlock();
    if (ptrblock == nullptr){
        ptrblock = mainblock.get();
    }
    double *val = ptrblock->getVarPtr(varname);
    blockfuncptr.addParentVar(varname, val);  //link return of func to current scope return*/
    std::unique_ptr<ScriptCommand> commandptr = std::make_unique<VarSetCommand>(*ptrblock, "return", varname);
    std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*commandptr.get());
    mainblock->addCommand(ref_cmd);
    commands.emplace_back(std::move(commandptr));
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
    std::cerr << "file : " << path_script << '\n';
    if (file_script.good()){
        char buffer[1000];
        std::cerr << "file good\n";
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
            std::cerr << nbline << " LINE : " << line << '\n';

            if (nbline == 38)std::cerr << "line.back : " << line.back() << ", line.size() : " << line.size() << '\n';

            bool entered = false;
            /* SYNTAXE TESTS */

            //Internal Functions
            parseInternalFunctions(entered, line, continuer);
            if (!entered && continuer){
                //Base Syntaxe
                parseLoops(entered, line, continuer);
                parseConditionBlocks(entered, line);
                if (!entered && continuer){
                    if (line.rfind("end", 0) == 0){
                        entered = true;
                        mainblock->addBlockEnd();
                    }else if (line.rfind("//", 0) == 0){
                        std::cerr << "com\n";
                        entered = true;
                    }else if (line.rfind("func", 0) == 0){
                        entered = true;
                        std::vector<std::string> vars;
                        std::string name;
                        line = line.substr(4, line.size()-5);
                        parseFuncStr(line, name, vars);
                        std::unique_ptr<ScriptBlock> block = std::make_unique<FunctionBlock>(name, vars);
                        std::reference_wrapper<ScriptBlock> ref_block = std::ref(*block.get());

                        std::unique_ptr<ScriptCommand> cmd_varset = std::make_unique<VarSetCommand>(ref_block.get(), "none", "none");
                        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*cmd_varset);
                        block->addNotPlayedCommand(ref_cmd);
                        commands.push_back(std::move(cmd_varset));

                        mainblock->addFunctionBlock(ref_block);
                        blocks.push_back(std::move(block));
                    }else if (std::string str = remove_ws(line); str.back() == ':'){
                        entered = true;
                        line.erase(remove_if(line.begin(), line.end(), [](auto t){return isspace(t)||t == ':' ;} ), line.end());
                        mainblock->addBlock(line);
                    }else if (line.rfind("return", 0) == 0){
                        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                        std::string expr = line.substr(6, line.size()-6);
                        std::cerr << "expr : " << expr << '\n';

                        ScriptBlock *ptrblock = mainblock->getCurBlock();

                        std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
                        if (ptrblock != nullptr){
                            ptrcommand = std::make_unique<VarSetCommand>(*ptrblock, expr, "return");
                        }else{
                            ptrcommand = std::make_unique<VarSetCommand>(*mainblock, expr, "return");
                        }
                        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
                        mainblock->addCommand(ref_cmd);
                        commands.emplace_back(std::move(ptrcommand));
                    }else if (std::string funcname = line.substr(0, line.find_first_of('(')); mainblock->functionExist(funcname) ){
                        std::vector<std::string> vars;
                        parseFuncStr(line, funcname, vars);
                        addBlockCallFunc(funcname, vars);
                    }else if (line.rfind("double ", 0) == 0) {
                        line.erase(remove_if(line.begin(), line.end(), isspace ), line.end());
                        line = line.substr(6, line.size()-6);
                        std::vector<std::string> tab = split(line, '=');
                        if (tab.size()!=2){
                            std::cerr << "segmentation fault : " << line << '\n';
                            continuer = false;
                            break;
                        }

                        std::string varname = tab[0];
                        std::string funcstr = tab[1];
                        std::string expr = tab[1];
                        std::vector<std::string> vars;
                        std::string func_name("");
                        parseFuncStr(funcstr, func_name, vars);
                        if (functionExist(func_name)){
                            if (!internalFunctionExist(func_name)){
                                createFuncVarSet(varname, func_name, vars);
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
                                std::cerr << "segmentation fault var " << varname << " not defined : " << line << '\n';
                                continuer = false;
                                break;
                            }
                            double val = 0;
                            if (mainblock->evalParserExpr(expr, val)){
                                std::cerr << "segmentation fault error during parsing : " << expr << '\n';
                                continuer = false;
                                break;
                            }
                            if (std::isnan(val)){
                                std::cerr << "segmentation fault string evaluated doesn't give a number : " << line << '\n';
                                continuer = false;
                                break;
                            }
                            ScriptBlock *ptrblock = mainblock->getCurBlock();
                            std::unique_ptr<ScriptCommand> ptrcommand = nullptr;
                            if (ptrblock != nullptr){
                                ptrcommand = std::make_unique<VarSetCommand>(*ptrblock, expr, varname);
                            }else{
                                ptrcommand = std::make_unique<VarSetCommand>(*mainblock, expr, varname);
                            }
                            std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*ptrcommand.get());
                            mainblock->addCommand(ref_cmd);
                            commands.push_back(std::move(ptrcommand));
                        }

                    }
                }
            }


            if (continuer && toaddcommand != nullptr && linetoaddcommand == nbline){
                std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*toaddcommand.get());
                mainblock->addCommand(ref_cmd);
                commands.emplace_back(std::move(toaddcommand));
                toaddcommand = nullptr;
            }
            /*-----SYNTAXE TEST END-----*/
        }
    }

    if (!continuer){
        std::cerr << "error during parsing !!\n";
    }
    else if (!mainblock->allBlocksEnded()){
        std::cerr << "error during parsing all blocks are not ended !\n";
    }else{
        std::cerr << "All blocks are correctly ended !! \n";
    }

    file_script.close();
    std::cerr << '\n' << '\n';
    std::cerr << "FILE SCRIPT " << path_script << " PARSED !!!\n";
    std::cerr << '\n';
}

ScriptReader::~ScriptReader()
{
    std::cerr << "\n\ndestruction script reader !!\n";
}
void ScriptReader::getCommands(std::size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand>> &pCommands){
    bool tmp;
    mainblock->getCommands(nbCommands, pCommands, tmp);
}

double ScriptReader::getVar(const std::string &var){
    return mainblock->getVar(var);
}

void ScriptReader::setVar(const std::string &var, double val){
    if (mainblock->varExist(var)){
        mainblock->addVar(var, val);
    }
}

void ScriptReader::addBlockCallFunc(const std::string &funcname, const std::vector<std::string> &exprs){
    if (mainblock->functionExist(funcname)){
        std::cerr << "adding func call of func " << funcname << '\n';
        auto curblock = mainblock->getCurBlock();
        if (curblock == nullptr)curblock = mainblock.get();

        auto &funcblock = mainblock->getFunction(funcname).get();

        auto callblock = std::make_unique<CallFuncBlock>(exprs, static_cast<FunctionBlock&>(funcblock));
        std::reference_wrapper<ScriptBlock> ref_callblock = std::ref(* static_cast<ScriptBlock*>(callblock.get()) );
        mainblock->addBlock(ref_callblock);
        blocks.push_back(std::move(callblock));

        double *val = curblock->getPersonalVarPtr("return");
        ref_callblock.get().addParentVar("return" , val);
        std::unique_ptr<ScriptCommand> cmd_varset = std::make_unique<VarSetCommand>(ref_callblock.get(), "return", "funcreturn");
        std::reference_wrapper<ScriptCommand> ref_cmd = std::ref(*cmd_varset);
        ref_callblock.get().addNotPlayedCommand(ref_cmd);
        commands.push_back(std::move(cmd_varset));

        mainblock->addBlockEnd();
    }else{
        std::cerr << "error script reader addblockcallfunc, funcname doesn't exist: " << funcname << '\n';
    }
}

void ScriptReader::initVars(const sf::Vector2f &pos, const Map &worldmap, float dt){
    mainblock->addVar("time", UniversalRobotsTime.getElapsedTime().asMilliseconds());
    mainblock->addVar("posx", static_cast<double>(pos.x));mainblock->addVar("posy", static_cast<double>(pos.y));
    mainblock->addVar("dt", static_cast<double>(dt));
    sf::Vector2f middlepos(pos.x+(TILE_SIZE/2), pos.y+(TILE_SIZE/2));
    int tmpx = static_cast<int>(middlepos.x) - (static_cast<int>(middlepos.x)%TILE_SIZE);tmpx/=TILE_SIZE;
    int tmpy = static_cast<int>(middlepos.y) - (static_cast<int>(middlepos.y)%TILE_SIZE);tmpy/=TILE_SIZE;
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
