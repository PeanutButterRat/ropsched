#ifndef ROPSCHED_X86_COMPARE_GADGET_INSTR_SCORE_H
#define ROPSCHED_X86_COMPARE_GADGET_INSTR_SCORE_H

#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/TargetInstrInfo.h"


struct X86CompareGadgetInstrScore {
  const llvm::TargetInstrInfo *TII;
  const llvm::TargetRegisterInfo *TRI;
  const unsigned *RD;

  enum class InstrCategory {
    DataMove,
    Arithmetic,
    ShiftAndRotate,
    Other,
  };

  enum class InstrDestinationReg {
    StackPointer,
    RD,
    Other,
  };

  explicit X86CompareGadgetInstrScore(const llvm::TargetInstrInfo *TII = nullptr,
    const llvm::TargetRegisterInfo *TRI = nullptr, const unsigned *RD = nullptr);

  bool operator() (llvm::SUnit *IA, llvm::SUnit *IB) const;

  float getInstrScore(llvm::SUnit *SU) const;

  InstrCategory getInstrCategory(llvm::MachineInstr *MI) const;

  InstrDestinationReg getInstrTarget(llvm::MachineInstr *MI) const;
};

#endif