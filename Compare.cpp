#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/TargetInstrInfo.h"


using namespace llvm;


constexpr std::array<StringLiteral, 6> DataMovePrefixes { "PUSH", "POP", "MOV", "XCHG", "LEA", "CMOV" };


struct Compare {
  const TargetInstrInfo *TII;

  enum InstrCategory {
    DataMove,
    Other,
  };

  explicit Compare(const TargetInstrInfo *TII = nullptr) : TII(TII) {}

  bool operator() (SUnit *IA, SUnit *IB) const {
    return getInstrScore(IA) <= getInstrScore(IB);
  }

  float getInstrScore(SUnit *SU) const {
    assert(TII && "TII is null");
    assert(SU && "SUnit is null");

    MachineInstr *MI = SU->getInstr();
    unsigned int Opcode = MI->getOpcode();
    StringRef Name = TII->getName(Opcode);
    InstrCategory Category = getInstrCategory(Name);

    switch (Category) {
      case DataMove:
        return 1.0f;
      default:
        return 0.0f;
    }
  }

  InstrCategory getInstrCategory(const StringRef Name) const {
    for (auto Prefix : DataMovePrefixes) {
      if (Name.starts_with(Prefix)) {
        return DataMove;
      }
    }

    return Other;
  }
};
