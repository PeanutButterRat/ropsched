#include "RopMachineScheduler.hpp"

using namespace llvm;


RopSchedStrategy::RopSchedStrategy(const MachineSchedContext *C) { }

void RopSchedStrategy::initialize(ScheduleDAGMI *DAG) {
  const X86CompareGadgetInstrScore Compare { DAG->TII, DAG->TRI, &this->RD };
  ReadyQ = PriorityQueue<SUnit *, std::vector<SUnit *>, X86CompareGadgetInstrScore>(Compare);
}

void RopSchedStrategy::enterMBB(llvm::MachineBasicBlock *MBB) {
  AssignedRD = false;
  RD = 0;
}

SUnit *RopSchedStrategy::pickNode(bool &IsTopNode) {
  if (ReadyQ.empty()) {
    return nullptr;
  }

  SUnit *Next = ReadyQ.top();
  ReadyQ.pop();
  IsTopNode = true;

  MachineInstr *MI = Next->getInstr();
  const MCInstrDesc &Desc = MI->getDesc();

  if (!AssignedRD) {
    for (unsigned Def = 0; Def < Desc.getNumDefs(); Def++) {
      MachineOperand Operand = MI->getOperand(Def);
      Register Reg = Operand.getReg();
      RD = Reg.id();
      AssignedRD = true;

      if (Register::isPhysicalRegister(Reg)) {
        break;
      }
    }
  }

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
