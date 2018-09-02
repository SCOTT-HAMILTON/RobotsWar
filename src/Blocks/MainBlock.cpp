#include "Blocks/MainBlock.h"
#include "Commands/BlockEntryCommand.h"

MainBlock::MainBlock() :
    ScriptBlock ("mainblock")
{

}

MainBlock::~MainBlock()
{

}

bool MainBlock::getCommands(size_t nbCommands, std::vector<std::reference_wrapper<ScriptCommand> > &pCommands, bool &commands_ended)
{
    bool commandsended = false;
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
    if (execInitCommands && !asloopblock)return true;

    if (asloopblock){
        if (!execInitCommands){
            commands_to_drop = &initOutLoopCommandsOrder;
        }
        else{
            if (loopblock != nullptr && loopblock->canEnter()){
                loopblock->getCommands(nbCommands, pCommands, commandsended);
                launched = true;
            }
        }
    }

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

                    std::cerr << "get commands of " << block.getType() << "\n";
                    /*std::cerr << "cmds before block get commands : \n";
                    for (auto &cmd : pCommands){
                        if (cmd.get().getType() == "varset")std::cerr << "varset cmd " << cmd.get().getStringProp("varname") << " = " << cmd.get().getStringProp("expr") << "\n";
                        else std::cerr << cmd.get().getType() << " cmd\n";
                    }std::cerr << "\n";
                    */
                    if (!block.isAddingBlockentryTestOnGetcmds()){
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
                std::cerr << "block " << type << "adding cmd " << cmd.get().getType() << '\n';
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
    return commandsended;
}

void MainBlock::addCommand(std::reference_wrapper<ScriptCommand> command)
{
    if (current_block.current_block != nullptr){
        std::cerr << "              NEW COMMAND " << command.get().getType() << " to " << current_block.current_block->getType() << '\n';
        current_block.current_block->addCommand(command);
    }
    else {
        if (command.get().getType() == "varset")std::cerr << "              NEW VARSET COMMAND " << command.get().getStringProp("varname") << "=" << command.get().getStringProp("expr") << " to me " << type << '\n';
        else std::cerr << "              NEW COMMAND " << command.get().getType() << " to me " << type << '\n';
        commandsorder.push_back(command);
        initOutLoopCommandsOrder.push_back(command);
    }
}

void MainBlock::addBlock(const std::string &blockstr)
{
    if (current_block.current_block != nullptr){
        current_block.current_block->addBlock(blockstr);
    }else{
        auto tmpblock = std::make_unique<ScriptBlock>(blockstr);
        std::reference_wrapper<ScriptBlock> ref_block = std::ref(*tmpblock);
        blocks.push_back( ref_block );
        ownedblocks.push_back( std::move(tmpblock) );

        for (auto it = vars.begin(); it != vars.end(); it++){
            if (it->first != "return")blocks.back().get().addMeParentVar(it->first, &it->second);
        }
        if (parentvars.size() != 0){
            for (auto it = parentvars.begin(); it != parentvars.end(); it++){
                if (it->first != "return")blocks.back().get().addMeParentVar(it->first, it->second);
            }
        }

        for (auto it = functions.begin(); it != functions.end(); it++){
            blocks.back().get().addMeParentFunction(it->first, it->second);
        }

        for (auto it = parentfunctions.begin(); it != parentfunctions.end(); it++){
            blocks.back().get().addMeParentFunction(it->first, it->second);
        }


        if (blockstr == std::string("loop")){
            loopblock = &blocks.back().get();
            asloopblock = true;
        }
        current_block.current_block = &blocks.back().get();
        current_block.nb_in_wait++;

        commandsorder.push_back(blocks.back());
        if (type == "mainblock" && !asloopblock){
            initOutLoopCommandsOrder.push_back(blocks.back());
        }

        std::cerr << "          ADD BLOCK (custom named block): " << blocks.back().get().getType() << '\n' << '\n';

        //vars test
          /*
        std::string varsstr("");
        for (auto it = blocks.back().get().vars.begin(); it != blocks.back().get().vars.end(); it++){
            varsstr += it->first;
            auto it2 = it;
            it2++;
            if (it2 != blocks.back().get().vars.end())varsstr += ", ";
        }
        std::cerr << "VARS : " << varsstr << '\n';

        varsstr = "";

        if (blocks.back().get().nbParentVars() != 0){
            if (varsstr != "")varsstr += ", ";
            for (size_t i = 0; i < blocks.back().get().nbParentVars(); i++){
                varsstr += it->first;
                auto it2 = it;
                it2++;
                if (it2 != blocks.back().get().parentvars.end())varsstr += ", ";
            }
        }
        std::cerr << "PARENTVARS : " << varsstr << '\n';*/

    }
}

void MainBlock::addBlock(std::reference_wrapper<ScriptBlock> block)
{
    if (current_block.current_block != nullptr){
        current_block.current_block->addBlock(block);
    }else{
        blocks.push_back(block);
        if (vars.size() != 0){
            for (auto it = vars.begin(); it != vars.end(); it++){
                if (it->first != "return")block.get().addMeParentVar(it->first, &it->second);
            }
        }
        if (parentvars.size() != 0){
            for (auto it = parentvars.begin(); it != parentvars.end(); it++){
                if (it->first != "return")block.get().addMeParentVar(it->first, it->second);
            }
        }
        for (auto it = functions.begin(); it != functions.end(); it++){
            block.get().addMeParentFunction(it->first, it->second);
        }
        for (auto it = parentfunctions.begin(); it != parentfunctions.end(); it++){
            block.get().addMeParentFunction(it->first, it->second);
        }

        if (block.get().getType() == "loop"){
            loopblock = &blocks.back().get();
            asloopblock = true;
        }
        current_block.current_block = &block.get();
        current_block.nb_in_wait++;
        commandsorder.push_back(block);
        if (type == "mainblock" && !asloopblock){
            initOutLoopCommandsOrder.push_back(block);
        }
        //vars test

        std::string varsstr("");/*
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
        }*/
    }
}

void MainBlock::addFunctionBlock(std::reference_wrapper<ScriptBlock> funcblock)
{
    std::cerr << "ADDING func block : " << funcblock.get().getName() << '\n';
    functions.emplace(funcblock.get().getName(), funcblock);

    if (vars.size() != 0){
        for (auto it = vars.begin(); it != vars.end(); it++){
            if (it->first != "return")funcblock.get().addMeParentVar(it->first, &it->second);
        }
    }

    for (auto it = functions.begin(); it != functions.end(); it++){
        funcblock.get().addMeParentFunction(it->first, it->second);
    }

    current_block.current_block = &funcblock.get();
    current_block.nb_in_wait++;
}
