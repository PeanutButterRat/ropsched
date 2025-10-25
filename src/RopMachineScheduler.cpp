#include "RopMachineScheduler.hpp"

using namespace llvm;


RopSchedStrategy::RopSchedStrategy(const MachineSchedContext *C) { }

void RopSchedStrategy::initialize(ScheduleDAGMI *DAG) {
  ReadyQ = PriorityQueue<SUnit *, std::vector<SUnit *>, X86CompareGadgetInstrScore>(X86CompareGadgetInstrScore(DAG->TII, DAG->TRI));
}

SUnit *RopSchedStrategy::pickNode(bool &IsTopNode) {
  if (ReadyQ.empty()) {
    return nullptr;
  }

  SUnit *Next = ReadyQ.top();
  ReadyQ.pop();
  IsTopNode = true;

  return Next;
}

void RopSchedStrategy::schedNode(SUnit *SU, bool IsTopNode) { }

void RopSchedStrategy::releaseTopNode(SUnit *SU) {
  ReadyQ.push(SU);
}

void RopSchedStrategy::releaseBottomNode(SUnit *SU) { }

static ScheduleDAGInstrs *createRopMachineScheduler(MachineSchedContext *C) {
  return new ScheduleDAGMILive(C, std::make_unique<RopSchedStrategy>(C));
}

static MachineSchedRegistry RopSchedRegistry("ropsched", "Return-oriented programming defensive scheduler", createRopMachineScheduler);
