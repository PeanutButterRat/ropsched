#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/ADT/PriorityQueue.h"
#include "Compare.cpp"

using namespace llvm;


class RopSchedStrategy : public MachineSchedStrategy {
  PriorityQueue<SUnit *, std::vector<SUnit *>, Compare> ReadyQ;

public:
  explicit RopSchedStrategy(const MachineSchedContext *C) { }

  void initialize(ScheduleDAGMI *DAG) override {
    ReadyQ = PriorityQueue<SUnit *, std::vector<SUnit *>, Compare>(Compare(DAG->TII));
  }

  SUnit *pickNode(bool &IsTopNode) override {
    if (ReadyQ.empty()) {
      return nullptr;
    }

    SUnit *Next = ReadyQ.top();
    ReadyQ.pop();
    IsTopNode = true;

    return Next;
  }

  void schedNode(SUnit *SU, bool IsTopNode) override { }

  void releaseTopNode(SUnit *SU) override {
    ReadyQ.push(SU);
  }

  void releaseBottomNode(SUnit *SU) override { }
};

static ScheduleDAGInstrs *createRopMachineScheduler(MachineSchedContext *C) {
  return new ScheduleDAGMILive(C, std::make_unique<RopSchedStrategy>(C));
}

static MachineSchedRegistry RopSchedRegistry("ropsched", "Return-oriented programming defensive scheduler", createRopMachineScheduler);
