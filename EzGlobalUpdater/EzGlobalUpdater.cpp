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

    std::cout << "Disassembling script " << Script->m_ScriptName << std::endl;

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
void TestScript(std::filesystem::path InPath, std::filesystem::path OutPath, std::uintptr_t SigAddr)
{
    auto LastTime = std::chrono::high_resolution_clock::now();
    auto Script = OpenScript(InPath, OutPath);
    auto CurrentTime = std::chrono::high_resolution_clock::now();

    std::cout << "Elapsed " << std::dec << std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - LastTime) << "\n" << std::endl;

    TestSignatureCreation(Script, SigAddr);
}

enum class EntryFuncVariables : std::uint32_t
{
    MyParam0,
    MyParam2,
    MyVar1
};

void EntryFuncHandler(Ez::EzPtr<Ez::EzAssemblerStateNode> Node)
{
    Node->Push<std::uint32_t, UINT32_MAX - UINT8_MAX - 1>(); /*force a 24bit value -> PUSH_CONST_U8_U8_U8*/
    Node->Push<std::uint8_t, UINT8_MAX>();
    Node->Sub(); /* auto Res = (UINT32_MAX - UINT8_MAX - 1) - UINT8_MAX */
    Node->IntToFloat(); /*auto CastedRes = float(Res);*/

    Node->Push<float, 27.f>(); 
    Node->FAdd(); /*auto Added = 27.f + CastedRes*/
    Node->Push<float, 1.f>();

    Node->Push<std::int16_t, -45>();
    Node->Push<std::int16_t, -25432>();
    Node->Add(); /*-45 + (-25432) = -25477*/
    Node->Native(0x234234324234, 1, 1); /*auto NativeRes = NativeCall(-25477)*/

    Node->Push<std::uint32_t, 1>(); 
    Node->LocalStore<std::uint32_t, 0>(); /*MyVar1 = 1;*/

    Node->Local<std::uint32_t, 234>();
    Node->Push<std::uint32_t, 24532>();
    Node->Add(); /*24532 + local value*/
    Node->Local<std::uint16_t, 2352>(); /*random garbage*/
    Node->Offset<std::uint8_t, 232>();
    Node->Global<std::uint32_t, UINT16_MAX + 1>();
}

void TestDisassembler()
{
    try
    {
        auto CurrentPath = std::filesystem::current_path();

        auto t1 = std::thread(&TestScript, 
            CurrentPath / "am_gang_call.ysc.full", 
            CurrentPath / "am_gang_call.txt",
            0xad);

        auto t2 = std::thread(&TestScript, 
            CurrentPath / "shop_controller.ysc.full", 
            CurrentPath / "shop_controller.txt",
            0xe309);

        auto t3 = std::thread(&TestScript, 
            CurrentPath / "gpb_tonya.ysc.full", 
            CurrentPath / "gpb_tonya.txt",
            0x143);

        t1.join();
        t2.join();
        t3.join();

        ScriptTable.print(std::cout);
    }
    catch (Ez::EzException& exp)
    {
        std::cout << "Error while disassembling script, error info: \t" << exp.what();
    }
}

void TestAssembler()
{
    try
    {
        auto Assembler = Ez::EzAssembler();

        auto EntryFunc = Assembler.CreateFunc("ENTRY", 0, 0);
        EntryFunc->SetHandler(&EntryFuncHandler);

        Assembler.Compile();
    }
    catch (Ez::EzException& exp)
    {
        std::cout << "Error while assembling script, error info: \t" << exp.what();
    }
}

int main()
{
    TestDisassembler();
    //TestAssembler();

    system("pause");
}
