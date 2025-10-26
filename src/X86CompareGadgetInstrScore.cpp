#include "llvm/CodeGen/Register.h"

#include "X86CompareGadgetInstrScore.hpp"


using namespace llvm;

constexpr std::array<StringLiteral, 6> DataMovePrefixes { "PUSH", "POP", "MOV", "XCHG", "LEA", "CMOV" };
constexpr std::array<StringLiteral, 13> ArithmeticPrefixes { "ADD", "SUB", "INC", "DEC", "SBB", "ADC", "MUL", "DIV", "IMUL", "IDIV", "XOR", "NEG", "NOT" };
constexpr std::array<StringLiteral, 8> ShiftAndRotatePrefixes { "SHL", "SHR", "SAR", "SAL", "ROR", "ROL", "RCR", "RCL" };

constexpr std::array<std::array<float, 3>, 4> ScoreTable {{
  { 2.0f, 1.0f, 0.5f },
  { 2.0f, 1.0f, 0.5f },
  { 3.0f, 2.0f, 0.5f },
  { 0.0f, 0.0f, 0.5f },
}};

X86CompareGadgetInstrScore::X86CompareGadgetInstrScore(const TargetInstrInfo *TII, const TargetRegisterInfo *TRI, const unsigned *RD)
: TII(TII), TRI(TRI), GadgetFirstInstrDestReg(RD) { }

bool X86CompareGadgetInstrScore::operator() (SUnit *IA, SUnit *IB) const {
  return getInstrScore(IA) <= getInstrScore(IB);
}

float X86CompareGadgetInstrScore::getInstrScore(SUnit *SU) const {
  assert(TII && "TII is null");
  assert(SU && "SUnit is null");

  MachineInstr *MI = SU->getInstr();
  InstrCategory Category = getInstrCategory(MI);
  InstrDestinationReg DestinationReg = getInstrTarget(MI);

  return ScoreTable[Category][DestinationReg];
}

X86CompareGadgetInstrScore::InstrCategory X86CompareGadgetInstrScore::getInstrCategory(MachineInstr *MI) const {
  unsigned int Opcode = MI->getOpcode();
  StringRef Name = TII->getName(Opcode);

  for (auto Prefix : DataMovePrefixes) {
    if (Name.starts_with(Prefix)) {
      return DataMove;
    }
  }

  for (auto Prefix : ArithmeticPrefixes) {
    if (Name.starts_with(Prefix)) {
      return Arithmetic;
    }
  }

  for (auto Prefix : ShiftAndRotatePrefixes) {
    if (Name.starts_with(Prefix)) {
      return ShiftAndRotate;
    }
  }

  return Unscored;
}

X86CompareGadgetInstrScore::InstrDestinationReg X86CompareGadgetInstrScore::getInstrTarget(MachineInstr *MI) const {
  const MCInstrDesc &Desc = MI->getDesc();
  InstrDestinationReg Destination = Other;

  for (size_t i = 0; i < Desc.getNumDefs(); i++) {
    MachineOperand Operand = MI->getOperand(i);
    Register Reg = Operand.getReg();

    if (Reg.id() == *GadgetFirstInstrDestReg) {
      Destination = GadgetFirstInstr;
    }

    if (Register::isPhysicalRegister(Reg)) {
      std::string Name { TRI->getName(Reg) };

      if (Name == "RSP") {
        return StackPointer;
      }
    }
  }

  return Destination;
}
