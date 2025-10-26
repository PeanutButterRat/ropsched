#include "llvm/CodeGen/Register.h"

#include "X86CompareGadgetInstrScore.hpp"

using namespace llvm;


constexpr std::array<std::array<float, 3>, 4> InstructionScoringTable {{
  { 2.0f, 1.0f, 0.5f },
  { 2.0f, 1.0f, 0.5f },
  { 3.0f, 2.0f, 0.5f },
  { 0.0f, 0.0f, 0.5f },
}};

const std::array<std::pair<X86CompareGadgetInstrScore::InstrCategory, std::vector<StringLiteral>>, 3> InstructionPrefixes {{
  {X86CompareGadgetInstrScore::DataMove, { "PUSH", "POP", "MOV", "XCHG", "LEA", "CMOV" }},
  {X86CompareGadgetInstrScore::Arithmetic, { "ADD", "SUB", "INC", "DEC", "SBB", "ADC", "MUL", "DIV", "IMUL", "IDIV", "XOR", "NEG", "NOT" }},
  {X86CompareGadgetInstrScore::ShiftAndRotate, { "SHL", "SHR", "SAR", "SAL", "ROR", "ROL", "RCR", "RCL" }},
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

  return InstructionScoringTable[Category][DestinationReg];
}

X86CompareGadgetInstrScore::InstrCategory X86CompareGadgetInstrScore::getInstrCategory(MachineInstr *MI) const {
  unsigned int Opcode = MI->getOpcode();
  StringRef Name = TII->getName(Opcode);

  for (auto [Category, Prefixes] : InstructionPrefixes) {
    for (auto Prefix : Prefixes) {
      if (Name.starts_with(Prefix)) {
        return Category;
      }
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
      const std::string Name { TRI->getName(Reg) };

      if (Name == "RSP") {
        return StackPointer;
      }
    }
  }

  return Destination;
}
