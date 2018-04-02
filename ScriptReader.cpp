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
    std::ifstream file_script;

    mup::ParserX parser;

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
            if (line.rfind("move(", 0) == 0) {
                line = line.substr(5, line.size()-6);
                std::cout << "params : " << line << std::endl;
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
                    std::cout << "var type x : " << line << std::endl;
                }else std::cout << "const type x : "<<line<< std::endl;
                if (std::isnan(y) || success_y == str_y){
                    offsety.type = VAR;
                    std::cout << "var type y : " << line << std::endl;
                }else std::cout << "const type y : "<<line<< std::endl;
                std::cout << "x, y : " << x << ", " << y << std::endl;
                ptr = std::make_shared<MoveCommand>(offsetx, offsety);
                commands.push_back(ptr);
            }else if (line.rfind("float ", 0) == 0) {
                line = line.substr(6, line.size()-6);
                std::vector<std::string> tab = split(line, '=');
                if (tab.size()!=2){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                parser.SetExpr(tab[1]);
                mup::Value mupval;
                std::string name = tab[0];
                std::cout << "try parsing : " << tab[1] << std::endl;
                try{
                    mupval = parser.Eval();
                }catch(mup::ParserError){
                    std::cout << "segmentation fault error during parsing : " << line << std::endl;
                    continuer = false;
                    break;
                }
                std::cout << "parsed " << std::endl;
                float val = mupval.GetFloat();
                std::cout << "val parsed !!!" << val << std::endl;
                if (std::isnan(val)){
                    std::cout << "segmentation fault : " << line << std::endl;
                    continuer = false;
                    break;
                }
                std::pair<std::string, std::variant<void*>> pair_var( name, std::variant<void*>(new float(val)) );
                vars.insert(pair_var);

                mupvars.insert(std::pair<std::string, mup::Value>(name, mup::Value(val)));
                mup::Variable var(&mupvars[name]);
                var.SetFloat(val);
                std::cout << "before !" << std::endl;
                parser.DefineVar(name, var);

                std::cout << "var val : " << *(float*)std::get<void*>(vars[name]) << std::endl;
                std::cout << "line : " << line << std::endl;
            }else if (struct {std::vector<std::string> tab; std::size_t x;} v = {split(line, '='), v.tab.size()}; v.x == 2 ){
                std::cout << "equality !!" << std::endl;
                if (!parser.IsVarDefined(v.tab[0])){
                    std::cout << "segmentation fault var : " << v.tab[0] << " not defined : " << line << std::endl;
                    continuer = false;
                    break;
                }
                parser.SetExpr(v.tab[1]);
                mup::Value mupval;
                try{
                    mupval = parser.Eval();
                }catch(mup::ParserError){
                    std::cout << "segmentation fault error during parsing : " << line << std::endl;
                    continuer = false;
                    break;
                }
                mup::Value *ptr = &mupvars[v.tab[0]];
                std::cout << "addr : " << ptr << std::endl;
                mupvars[v.tab[0]] = mupval;
                std::cout << "addr : " << ptr << std::endl;
                std::cout << "new value for " << v.tab[0] << " : " << mupval.ToString() << std::endl;
            }
            /*-----SYNTAXE TEST END-----*/
        }

    }

    file_script.close();
}

ScriptReader::~ScriptReader()
{
    //dtor
}
void ScriptReader::getCommands(std::size_t start, std::size_t nbCommands, std::vector<std::weak_ptr<ScriptCommand>> &pCommands){
    for (std::size_t i = start; i < nbCommands+start && i < commands.size(); i++){
        pCommands.push_back( std::weak_ptr<ScriptCommand>(commands[i]) );
    }
}

int ScriptReader::nbCommands(){
    return commands.size();
}

const void* ScriptReader::getVar(const std::string &var){
    return std::get<void*>(vars[var]);
}
