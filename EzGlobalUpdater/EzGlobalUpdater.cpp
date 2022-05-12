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

void AddNewScript2Table(Ez::EzDisassembler* Disassembler)
{
    ScriptTable.addRow(Disassembler->GetScriptName(), Disassembler->GetLocals().size(),
        Disassembler->GetScriptParams().size(), Disassembler->GetStrings().size(),
        Disassembler->GetNatives().size(), Disassembler->GetBlocksNumber(), Disassembler->GetCodeSize(),
        Disassembler->GetFunctions().size(), Disassembler->GetMappedInstructions());
}

void TestSignatureCreation(Ez::EzDisassembler* Disassembler, std::uintptr_t Addr)
{
    auto Func = Disassembler->GetFuncFromOffset(Addr);
    auto& Instrs = const_cast<std::vector<std::shared_ptr<Ez::EzInstruction>>&>
        (Func->GetInstructions());

    auto Signature = Ez::EzSignature();
    Signature.SetSignatureContext(Disassembler->GetCode(), Disassembler->GetCodeSize(),
        Instrs.data(), Instrs.size(), Ez::rage::RageResourceVersion::RRV_MinusThan10);

    Signature.CreateAtAddress(Addr);
    auto SigStr = Signature.GetSignature();

    auto Found = Signature.ScanFromNewSig(SigStr);

    std::cout << "Created signature of script " << Disassembler->GetScriptName() << " at : 0x" << std::hex << Addr
        << "\t: " << Signature.GetSignature() << std::endl;
}

Ez::EzDisassembler* OpenScript(std::filesystem::path InPath, std::filesystem::path OutPath)
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
    Script->ParseHeader(InStream, "");

    auto Disassembler = new Ez::EzDisassembler(Script);
    Disassembler->MapScript();
    Disassembler->Disassemble();

    auto& Assembly = Disassembler->GetAssembly();
    OutStream << Assembly.str() << std::endl;

    AddNewScript2Table(Disassembler);

    Disassembler->FreeDisassemblyBuffer();
    InStream.close();
    OutStream.close();

    return Disassembler;
}

void UpdateGlobals()
{
    try
    {
        auto CurrentPath = std::filesystem::current_path();

        auto Script1 = OpenScript(CurrentPath / "am_gang_call.ysc.full",
            CurrentPath / "am_gang_call_disam.txt");

        auto Script2 = OpenScript(CurrentPath / "gpb_tonya.ysc.full",
            CurrentPath / "gpb_tonya.txt");

        auto Script3 = OpenScript(CurrentPath / "shop_controller.ysc.full",
            CurrentPath / "shop_controller.txt");

        TestSignatureCreation(Script1, 0xad);
        TestSignatureCreation(Script2, 0x143);
        TestSignatureCreation(Script3, 0xe309);

        ScriptTable.print(std::cout);
    }
    catch (Ez::EzException& exp)
    {
        std::cout << "Error while disassembling script, error info: \t" << exp.what();
    }
}

int main()
{
    UpdateGlobals();

    system("pause");
}
