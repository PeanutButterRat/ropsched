#include "llvm/CodeGen/MachineScheduler.h"

#include <iostream>


class RopSchedStrategy : public llvm::GenericScheduler {
public:
  RopSchedStrategy(const llvm::MachineSchedContext *C) : llvm::GenericScheduler(C) {
    std::cout << "[RopSchedStrategy] Instantiated." << std::endl;
  }

  void initialize(llvm::ScheduleDAGMI *DAG) override {
    std::cout << "[RopSchedStrategy] Initializing DAG." << std::endl;
    llvm::GenericScheduler::initialize(DAG);
  }

  llvm::SUnit *pickNode(bool &IsTopNode) override {
    std::cout << "[RopSchedStrategy] Picking node." << std::endl;
    return GenericScheduler::pickNode(IsTopNode);
  }

  void schedNode(llvm::SUnit *SU, bool IsTopNode) override {
    std::cout << "[RopSchedStrategy] Scheduling node." << std::endl;
    llvm::GenericScheduler::schedNode(SU, IsTopNode);
  }
};

static llvm::ScheduleDAGInstrs *createRopMachineScheduler(llvm::MachineSchedContext *C) {
  return new llvm::ScheduleDAGMILive(C, std::make_unique<RopSchedStrategy>(C));
}

static llvm::MachineSchedRegistry RopSchedRegistry("ropsched", "Return-oriented programming defensive scheduler", createRopMachineScheduler);
