#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/Register.h"

using namespace llvm;


constexpr std::array<StringLiteral, 6> DataMovePrefixes { "PUSH", "POP", "MOV", "XCHG", "LEA", "CMOV" };
constexpr std::array<StringLiteral, 13> ArithmeticPrefixes { "ADD", "SUB", "INC", "DEC", "SBB", "ADC", "MUL", "DIV", "IMUL", "IDIV", "XOR", "NEG", "NOT" };
constexpr std::array<StringLiteral, 8> ShiftAndRotatePrefixes { "SHL", "SHR", "SAR", "SAL", "ROR", "ROL", "RCR", "RCL" };

struct Compare {
  const TargetInstrInfo *TII;
  const TargetRegisterInfo *TRI;

  enum class InstrCategory {
    DataMove,
    Arithmetic,
    ShiftAndRotate,
    Other,
  };

  enum class InstrDestinationReg {
    StackPointer,
    Other,
  };

  explicit Compare(const TargetInstrInfo *TII = nullptr, const TargetRegisterInfo *TRI = nullptr) : TII(TII), TRI(TRI) {}

  bool operator() (SUnit *IA, SUnit *IB) const {
    return getInstrScore(IA) <= getInstrScore(IB);
  }

  float getInstrScore(SUnit *SU) const {
    assert(TII && "TII is null");
    assert(SU && "SUnit is null");

    MachineInstr *MI = SU->getInstr();
    InstrCategory Category = getInstrCategory(MI);
    InstrDestinationReg DestinationReg = getInstrTarget(MI);

    switch (Category) {
      case InstrCategory::DataMove:
        return (DestinationReg == InstrDestinationReg::StackPointer) ? 2.0f : 1.0f;
      case InstrCategory::Arithmetic:
        return (DestinationReg == InstrDestinationReg::StackPointer) ? 2.0f : 1.0f;
      case InstrCategory::ShiftAndRotate:
        return (DestinationReg == InstrDestinationReg::StackPointer) ? 3.0f : 1.0f;
      default:
        return 0.0f;
    }
  }

  InstrCategory getInstrCategory(MachineInstr *MI) const {
    unsigned int Opcode = MI->getOpcode();
    StringRef Name = TII->getName(Opcode);

    for (auto Prefix : DataMovePrefixes) {
      if (Name.starts_with(Prefix)) {
        return InstrCategory::DataMove;
      }
    }

    for (auto Prefix : ArithmeticPrefixes) {
      if (Name.starts_with(Prefix)) {
        return InstrCategory::Arithmetic;
      }
    }

    for (auto Prefix : ShiftAndRotatePrefixes) {
      if (Name.starts_with(Prefix)) {
        return InstrCategory::ShiftAndRotate;
      }
    }

    return InstrCategory::Other;
  }

  InstrDestinationReg getInstrTarget(MachineInstr *MI) const {
    const MCInstrDesc &Desc = MI->getDesc();

    for (size_t i = 0; i < Desc.getNumDefs(); i++) {
      MachineOperand Operand = MI->getOperand(i);
      Register Reg = Operand.getReg();

      if (Register::isPhysicalRegister(Reg)) {
        std::string Name { TRI->getName(Reg) };

        if (Name == "RSP") {
          return InstrDestinationReg::StackPointer;
        }
      }
    }

    return InstrDestinationReg::Other;
  }
};
