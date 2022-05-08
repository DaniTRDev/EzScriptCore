// EzGlobalUpdater.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>

#define EZGLOBAL_INCLUDES
#define EZGLOBAL_IMPL

//#include "EzGlobalUpdater.h"

#include <include/VariadicTable.h>
#include "pch.h"

using ScriptTable_t = VariadicTable<std::string, std::size_t, std::size_t, std::size_t,
    std::size_t, std::size_t, std::size_t, std::size_t, std::size_t>;

ScriptTable_t ScriptTable({ "Name", "Locals", "Script Params", "String", "Natives",
        "Blocks", "Total Code Lenght", "Functions", "Instructions" });

void AddNewScript2Table(Ez::EzDecompiler* Decompiler)
{
    ScriptTable.addRow(Decompiler->GetScriptName(), Decompiler->GetLocals().size(), 
        Decompiler->GetScriptParams().size(), Decompiler->GetStrings().size(), 
        Decompiler->GetNatives().size(), Decompiler->GetBlocksNumber(), Decompiler->GetCodeSize(),
        Decompiler->GetFunctions().size(), Decompiler->GetMappedInstructions());
}

void TestSignatureCreation(Ez::EzDecompiler& Decompiler)
{
    auto TestAddr = 0xad;
    auto Func = Decompiler.GetFuncFromOffset(TestAddr);
    auto& Instrs = const_cast<std::vector<std::shared_ptr<Ez::EzInstruction>>&>
        (Func->GetInstructions());

    auto Signature = Ez::EzSignature();
    Signature.SetSignatureContext(Decompiler.GetCode(), Decompiler.GetCodeSize(),
        Instrs.data(), Instrs.size(), Ez::rage::RageResourceVersion::RRV_MinusThan10);

    auto Res = Signature.CreateAtAddress(TestAddr);
    auto SigStr = Signature.GetSignature();

    auto Found = Signature.ScanFromNewSig(SigStr);

    std::cout << "Created signature at: 0x" << std::hex << TestAddr
        << "\t: " << Signature.GetSignature() << std::endl;
}

Ez::EzDecompiler * OpenScript(std::filesystem::path InPath, std::filesystem::path OutPath)
{
    std::ifstream InStream(InPath.string(), std::ios::in | std::ios::binary);
    std::ofstream OutStream(OutPath.string(), std::ios::out | std::ios::binary | std::ios::trunc);

    if (!InStream.is_open())
    {
        std::cout << "Could not open input file, check path: " << InPath.string() << std::endl;
        return nullptr;
    }
    else if (!OutStream.is_open())
    {
        std::cout << "Could not open output file, check path: " << OutPath.string() << std::endl;
        return nullptr;
    }

    auto Script = new Ez::EzSrcProgram();

    if (auto Result = Script->ParseHeader(InStream, ""); Result != Ez::EzScriptStatus::NoError)
    {
        std::cout << "Error " << static_cast<std::uint32_t>(Result) << std::endl;
        return nullptr;
    }

    auto Decompiler = new Ez::EzDecompiler(Script);

    if (auto Result = Decompiler->MapScript(); Result != Ez::EzDecompilerStatus::NoError)
        std::cout << "Error " << static_cast<std::uint32_t>(Result) << std::endl;

    if (auto Result = Decompiler->Disassemble(); Result != Ez::EzDecompilerStatus::NoError)
        std::cout << "Error " << static_cast<std::uint32_t>(Result) << std::endl;

    auto& Assembly = Decompiler->GetAssembly();
    OutStream << Assembly.str() << std::endl;

    AddNewScript2Table(Decompiler);

    Decompiler->FreeDisassemblyBuffer();
    InStream.close();
    OutStream.close();

    return Decompiler;
}

void UpdateGlobals()
{
    auto CurrentPath = std::filesystem::current_path();

    auto Script1 =  OpenScript(CurrentPath / "am_gang_call.ysc.full", 
        CurrentPath / "am_gang_call_disam.txt");

    auto Script2 = OpenScript(CurrentPath / "gpb_tonya.ysc.full",
        CurrentPath / "gpb_tonya.txt");

    auto Script3 = OpenScript(CurrentPath / "shop_controller.ysc.full",
        CurrentPath / "shop_controller.txt");

    ScriptTable.print(std::cout);
}

int main()
{
    UpdateGlobals();

    system("pause");
}
