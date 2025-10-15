#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/Support/raw_ostream.h"

#include <queue>


class Compare {
public:
  bool operator() (llvm::SUnit *, llvm::SUnit *) {
    return true;
  }
};


class RopSchedStrategy : public llvm::MachineSchedStrategy {
  std::priority_queue<llvm::SUnit *, std::vector<llvm::SUnit *>, Compare> ReadyQ;

public:
  RopSchedStrategy(const llvm::MachineSchedContext *C) : llvm::MachineSchedStrategy() { }

  void initialize(llvm::ScheduleDAGMI *DAG) override { }

  llvm::SUnit *pickNode(bool &IsTopNode) override {
    llvm::outs() << "[RopSchedStrategy] Picking node.\n";

    if (ReadyQ.empty()) {
      return nullptr;
    }

    llvm::SUnit *next = ReadyQ.top();
    ReadyQ.pop();
    IsTopNode = true;

    return next;
  }

  void schedNode(llvm::SUnit *SU, bool IsTopNode) override { }

  void releaseTopNode(llvm::SUnit *SU) override {
    llvm::outs() << "[RopSchedStrategy] Release Top node.\n";
    ReadyQ.push(SU);
  }

  void releaseBottomNode(llvm::SUnit *SU) override { }
};

static llvm::ScheduleDAGInstrs *createRopMachineScheduler(llvm::MachineSchedContext *C) {
  return new llvm::ScheduleDAGMILive(C, std::make_unique<RopSchedStrategy>(C));
}

static llvm::MachineSchedRegistry RopSchedRegistry("ropsched", "Return-oriented programming defensive scheduler", createRopMachineScheduler);
