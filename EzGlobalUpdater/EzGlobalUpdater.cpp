// EzGlobalUpdater.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>

#define EZGLOBAL_INCLUDES
#define EZGLOBAL_IMPL

//#include "EzGlobalUpdater.h"

#include "pch.h"

void UpdateGlobals()
{
    //Ez::EzScriptFile ScrFile("C:\\Users\\danit\\source\\repos\\EzGlobalUpdater\\x64\\Release\\am_gang_call.ysc.full");

    std::ifstream Stream;
    Stream.open("C:\\Users\\danit\\source\\repos\\EzGlobalUpdater\\x64\\Release\\am_gang_call.ysc.full", std::fstream::in | std::fstream::binary);
   
    Ez::EzSrcProgram Script;
    if (auto Result = Script.ParseHeader(Stream, "am_gang_call", false); Result != Ez::EzScriptStatus::NoError)
        std::cout << "Error " << static_cast<std::uint32_t>(Result) << std::endl;

    Ez::EzDecompiler Decompiler(&Script);

    if (auto Result = Decompiler.MapLocals(); Result != Ez::EzDecompilerStatus::NoError)
        std::cout << "Error " << static_cast<std::uint32_t>(Result) << std::endl;

    printf("Mapped %zu locals\n", Decompiler.GetLocals().size());
    printf("Mapped %zu script params\n", Decompiler.GetScriptParams().size());

    if (auto Result = Decompiler.MapStrings(); Result != Ez::EzDecompilerStatus::NoError)
        std::cout << "Error " << static_cast<std::uint32_t>(Result) << std::endl;

    printf("Mapped %zu strings\n", Decompiler.GetStrings().size());

    if (auto Result = Decompiler.MapCodeBlocks(); Result != Ez::EzDecompilerStatus::NoError)
        std::cout << "Error " << static_cast<std::uint32_t>(Result) << std::endl;

    printf("Mapped %zu blocks -> Merged in a single block of size 0x%zx\n", 
            Decompiler.GetBlocksNumber(), Decompiler.GetBlockSize());

    if (auto Result = Decompiler.MapFunctions(); Result != Ez::EzDecompilerStatus::NoError)
        std::cout << "Error " << static_cast<std::uint32_t>(Result) << std::endl;

    printf("Mapped %zu functions\n", Decompiler.GetFunctions().size());

    if (auto Result = Decompiler.PreDecompileFunctions(); Result != Ez::EzDecompilerStatus::NoError)
        std::cout << "Error " << static_cast<std::uint32_t>(Result) << std::endl;

    printf("Predecompiled functions, generated %zu instructions\n", Decompiler.GetDecompiledInstructions());

    /*if (!ScrFile.ParseHeader("freemode"))
    {
        std::cout << "Error while parsing the header!" << std::endl;
        return;
    }

    if (!ScrFile.MapLocals())
    {
        std::cout << "Error while mapping locals!" << std::endl;
        return;
    }

    if (!ScrFile.MapCodeBlocks())
    {
        std::cout << "Error while mapping code blocks!" << std::endl;
        return;
    }

    if (!ScrFile.MapFunctions())
    {
        std::cout << "Error while mapping functions from blocks!" << std::endl;
        return;
    }

    if (!ScrFile.MapFuncsInstructions())
    {
        std::cout << "Error while mapping instructions from functions!" << std::endl;
        return;
    }

    if (!ScrFile.DecompileFuncsInstructions())
    {
        std::cout << "Error while decompiling instructions from functions!" << std::endl;
        return;
    }*/
}

int main()
{
    UpdateGlobals();

    system("pause");
}
