#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/TargetInstrInfo.h"


using namespace llvm;




struct Compare {
  const TargetInstrInfo *TII;

  explicit Compare(const TargetInstrInfo *TII = nullptr) : TII(TII) {}

  bool operator() (SUnit *A, SUnit *B) const {
    return getInstructionScore(A) <= getInstructionScore(B);
  }

  float getInstructionScore(SUnit *SU) const {
    assert(TII && "TII is null");
    assert(SU && "SUnit is null");

    MachineInstr *MI = SU->getInstr();
    unsigned int Opcode = MI->getOpcode();
    StringRef Name = TII->getName(Opcode);

    if (Name.starts_with("ADD")) {
      return 1.0f;
    }

    return 0.0f;
  }
};
