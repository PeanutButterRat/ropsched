#ifndef ROPSCHED_ROPMACHINESCHEDULER_H
#define ROPSCHED_ROPMACHINESCHEDULER_H

#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/ADT/PriorityQueue.h"

#include "X86CompareGadgetInstrScore.hpp"


class RopSchedStrategy : public llvm::MachineSchedStrategy {
  llvm::PriorityQueue<llvm::SUnit *, std::vector<llvm::SUnit *>, X86CompareGadgetInstrScore> ReadyQ;

public:
  explicit RopSchedStrategy(const llvm::MachineSchedContext *C);

  void initialize(llvm::ScheduleDAGMI *DAG) override;

  llvm::SUnit *pickNode(bool &IsTopNode) override;

  void schedNode(llvm::SUnit *SU, bool IsTopNode) override;

  void releaseTopNode(llvm::SUnit *SU) override;

  void releaseBottomNode(llvm::SUnit *SU) override;
};

#endif
