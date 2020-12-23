/******************************************************************************
  GrooveStomp's GB Emulator
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: cpu_impl.cpp
  Created: 2019-09-07
  Updated: 2020-12-23
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)

  This program comes with ABSOLUTELY NO WARRANTY. This is free software, and
  you are welcome to redistribute it under certain conditions; See LICENSE for
  details.
 ******************************************************************************/

//! \file cpu_impl.cpp

#include <cstdint>
#include <memory>
#include <string>
#include <map>

#include "cpu.hpp"

namespace gs {

class bus;
class operand;
class instruction;

class cpu::impl {
public:
        impl(cpu *cpuIn, bus *busIn);
        ~impl();

        void InitInstructionMap();

        // Load operations
        void LD();
        void LDD();
        void LDI();
        void LDH();
        void LDHL();

        // Arithmetic/logic operations
        void PUSH();
        void POP();
        void ADD8();
        void ADC8();
        void SUB8();
        void SBC8();
        void AND();
        void OR();
        void XOR();
        void CP();
        void INC8();
        void DEC8();
        void ADD16();
        void INC16();
        void DEC16();

        // Miscellaneous operations
        void SWAP();
        void DAA();
        void CPL();
        void CCF();
        void SCF();
        void NOP();
        void HALT();
        void STOP();
        void DI();
        void EI();

        // Rotate and shift operations
        void RLCA();
        void RLA();
        void RRCA();
        void RRA();
        void RLC();
        void RL();
        void RRC();
        void RR();
        void SLA();
        void SRA();
        void SRL();

        // Bit operations
        void BIT();
        void SET();
        void RES();

        void JP(); // Jump
        void JR(); // Jump
        void CALL(); // Call
        void RST(); // Restart
        void RET(); // Return
        void RETI(); // Return, enabling interrupts

        // Instance variables
        cpu *mCpu;
        bus *mBus;
        bool halted = false;
        bool waitForButtonPress = false;
        bool interruptsDisabled = false;
        bool interruptsDisabledRequested = false;
        bool interruptsEnabledRequested = false;

        // Variables and functions to assist in emulation
        std::shared_ptr<operand> operand1;
        std::shared_ptr<operand> operand2;
        std::shared_ptr<instruction> mInstruction = nullptr;
        std::map<int,instruction> instructionMap;

        // Opcodes
        void Op_0000();
        void Op_0001();
        void Op_0002();
        void Op_0003();
        void Op_0004();
        void Op_0005();
        void Op_0006();
        void Op_0007();
        void Op_0008();
        void Op_0009();
        void Op_000A();
        void Op_000B();
        void Op_000C();
        void Op_000D();
        void Op_000E();
        void Op_000F();
        void Op_0011();
        void Op_0012();
        void Op_0013();
        void Op_0014();
        void Op_0015();
        void Op_0016();
        void Op_0017();
        void Op_0018();
        void Op_0019();
        void Op_001A();
        void Op_001B();
        void Op_001C();
        void Op_001D();
        void Op_001E();
        void Op_001F();
        void Op_0020();
        void Op_0021();
        void Op_0022();
        void Op_0023();
        void Op_0024();
        void Op_0025();
        void Op_0026();
        void Op_0027();
        void Op_0028();
        void Op_0029();
        void Op_002A();
        void Op_002B();
        void Op_002C();
        void Op_002D();
        void Op_002E();
        void Op_002F();
        void Op_0030();
        void Op_0031();
        void Op_0032();
        void Op_0033();
        void Op_0034();
        void Op_0035();
        void Op_0036();
        void Op_0037();
        void Op_0038();
        void Op_0039();
        void Op_003A();
        void Op_003B();
        void Op_003C();
        void Op_003D();
        void Op_003E();
        void Op_003F();
        void Op_0040();
        void Op_0041();
        void Op_0042();
        void Op_0043();
        void Op_0044();
        void Op_0045();
        void Op_0046();
        void Op_0047();
        void Op_0048();
        void Op_0049();
        void Op_004A();
        void Op_004B();
        void Op_004C();
        void Op_004D();
        void Op_004E();
        void Op_004F();
        void Op_0050();
        void Op_0051();
        void Op_0052();
        void Op_0053();
        void Op_0054();
        void Op_0055();
        void Op_0056();
        void Op_0057();
        void Op_0058();
        void Op_0059();
        void Op_005A();
        void Op_005B();
        void Op_005C();
        void Op_005D();
        void Op_005E();
        void Op_005F();
        void Op_0060();
        void Op_0061();
        void Op_0062();
        void Op_0063();
        void Op_0064();
        void Op_0065();
        void Op_0066();
        void Op_0067();
        void Op_0068();
        void Op_0069();
        void Op_006A();
        void Op_006B();
        void Op_006C();
        void Op_006D();
        void Op_006E();
        void Op_006F();
        void Op_0070();
        void Op_0071();
        void Op_0072();
        void Op_0073();
        void Op_0074();
        void Op_0075();
        void Op_0076();
        void Op_0077();
        void Op_0078();
        void Op_0079();
        void Op_007A();
        void Op_007B();
        void Op_007C();
        void Op_007D();
        void Op_007E();
        void Op_007F();
        void Op_0080();
        void Op_0081();
        void Op_0082();
        void Op_0083();
        void Op_0084();
        void Op_0085();
        void Op_0086();
        void Op_0087();
        void Op_0088();
        void Op_0089();
        void Op_008A();
        void Op_008B();
        void Op_008C();
        void Op_008D();
        void Op_008E();
        void Op_008F();
        void Op_0090();
        void Op_0091();
        void Op_0092();
        void Op_0093();
        void Op_0094();
        void Op_0095();
        void Op_0096();
        void Op_0097();
        void Op_0098();
        void Op_0099();
        void Op_009A();
        void Op_009B();
        void Op_009C();
        void Op_009D();
        void Op_009E();
        void Op_009F();
        void Op_00A0();
        void Op_00A1();
        void Op_00A2();
        void Op_00A3();
        void Op_00A4();
        void Op_00A5();
        void Op_00A6();
        void Op_00A7();
        void Op_00A8();
        void Op_00A9();
        void Op_00AA();
        void Op_00AB();
        void Op_00AC();
        void Op_00AD();
        void Op_00AE();
        void Op_00AF();
        void Op_00B0();
        void Op_00B1();
        void Op_00B2();
        void Op_00B3();
        void Op_00B4();
        void Op_00B5();
        void Op_00B6();
        void Op_00B7();
        void Op_00B8();
        void Op_00B9();
        void Op_00BA();
        void Op_00BB();
        void Op_00BC();
        void Op_00BD();
        void Op_00BE();
        void Op_00BF();
        void Op_00C0();
        void Op_00C1();
        void Op_00C2();
        void Op_00C3();
        void Op_00C4();
        void Op_00C5();
        void Op_00C6();
        void Op_00C7();
        void Op_00C8();
        void Op_00C9();
        void Op_00CA();
        void Op_00CC();
        void Op_00CD();
        void Op_00CE();
        void Op_00CF();
        void Op_00D0();
        void Op_00D1();
        void Op_00D2();
        void Op_00D4();
        void Op_00D5();
        void Op_00D6();
        void Op_00D7();
        void Op_00D8();
        void Op_00D9();
        void Op_00DA();
        void Op_00DC();
        void Op_00DF();
        void Op_00E0();
        void Op_00E1();
        void Op_00E2();
        void Op_00E5();
        void Op_00E6();
        void Op_00E7();
        void Op_00E8();
        void Op_00E9();
        void Op_00EA();
        void Op_00EE();
        void Op_00EF();
        void Op_00F0();
        void Op_00F1();
        void Op_00F2();
        void Op_00F3();
        void Op_00F5();
        void Op_00F6();
        void Op_00F7();
        void Op_00F8();
        void Op_00F9();
        void Op_00FA();
        void Op_00FB();
        void Op_00FE();
        void Op_00FF();
        void Op_1000();
        void Op_CB00();
        void Op_CB01();
        void Op_CB02();
        void Op_CB03();
        void Op_CB04();
        void Op_CB05();
        void Op_CB06();
        void Op_CB07();
        void Op_CB08();
        void Op_CB09();
        void Op_CB0A();
        void Op_CB0B();
        void Op_CB0C();
        void Op_CB0D();
        void Op_CB0E();
        void Op_CB0F();
        void Op_CB10();
        void Op_CB11();
        void Op_CB12();
        void Op_CB13();
        void Op_CB14();
        void Op_CB15();
        void Op_CB16();
        void Op_CB17();
        void Op_CB18();
        void Op_CB19();
        void Op_CB1A();
        void Op_CB1B();
        void Op_CB1C();
        void Op_CB1D();
        void Op_CB1E();
        void Op_CB1F();
        void Op_CB20();
        void Op_CB21();
        void Op_CB22();
        void Op_CB23();
        void Op_CB24();
        void Op_CB25();
        void Op_CB26();
        void Op_CB27();
        void Op_CB28();
        void Op_CB29();
        void Op_CB2A();
        void Op_CB2B();
        void Op_CB2C();
        void Op_CB2D();
        void Op_CB2E();
        void Op_CB2F();
        void Op_CB30();
        void Op_CB31();
        void Op_CB32();
        void Op_CB33();
        void Op_CB34();
        void Op_CB35();
        void Op_CB36();
        void Op_CB37();
        void Op_CB38();
        void Op_CB39();
        void Op_CB3A();
        void Op_CB3B();
        void Op_CB3C();
        void Op_CB3D();
        void Op_CB3E();
        void Op_CB3F();
        void Op_CB40();
        void Op_CB41();
        void Op_CB42();
        void Op_CB43();
        void Op_CB44();
        void Op_CB45();
        void Op_CB46();
        void Op_CB47();
        void Op_CB80();
        void Op_CB81();
        void Op_CB82();
        void Op_CB83();
        void Op_CB84();
        void Op_CB85();
        void Op_CB86();
        void Op_CB87();
        void Op_CBC0();
        void Op_CBC1();
        void Op_CBC2();
        void Op_CBC3();
        void Op_CBC4();
        void Op_CBC5();
        void Op_CBC6();
        void Op_CBC7();
        void Op_CBC8();
};

} // namespace gs
