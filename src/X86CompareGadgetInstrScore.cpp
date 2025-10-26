#include "llvm/CodeGen/Register.h"

#include "X86CompareGadgetInstrScore.hpp"

using namespace llvm;

typedef X86CompareGadgetInstrScore::InstrCategory InstrCategory;
typedef X86CompareGadgetInstrScore::InstrDestinationReg InstrDestinationReg;


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

bool X86CompareGadgetInstrScore::operator() (const SUnit *IA, const SUnit *IB) const {
  return getInstrScore(IA) <= getInstrScore(IB);
}

float X86CompareGadgetInstrScore::getInstrScore(const SUnit *SU) const {
  assert(TII && "TII is null");
  assert(SU && "SUnit is null");

  const MachineInstr *MI = SU->getInstr();
  const InstrCategory Category = getInstrCategory(MI);
  const InstrDestinationReg DestinationReg = getInstrTarget(MI);

  return InstructionScoringTable[Category][DestinationReg];
}

InstrCategory X86CompareGadgetInstrScore::getInstrCategory(const MachineInstr *MI) const {
  const unsigned Opcode = MI->getOpcode();
  const StringRef Name = TII->getName(Opcode);

  for (const auto& [Category, Prefixes] : InstructionPrefixes) {
    for (const auto& Prefix : Prefixes) {
      if (Name.starts_with(Prefix)) {
        return Category;
      }
    }
  }

  return Unscored;
}

InstrDestinationReg X86CompareGadgetInstrScore::getInstrTarget(const MachineInstr *MI) const {
  const MCInstrDesc &Desc = MI->getDesc();
  InstrDestinationReg Destination = Other;

  for (size_t i = 0; i < Desc.getNumDefs(); i++) {
    const MachineOperand Operand = MI->getOperand(i);
    const Register Reg = Operand.getReg();

    if (Reg.id() == *GadgetFirstInstrDestReg) {
      Destination = GadgetFirstInstr;
    }

    if (Register::isPhysicalRegister(Reg)) {
      const StringRef Name { TRI->getName(Reg) };

      if (Name == "RSP") {
        return StackPointer;
      }
    }
  }

  return Destination;
}
