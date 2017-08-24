#ifndef SCHEDULEPLAN_HPP
#define SCHEDULEPLAN_HPP

#include <vector>
#include <string>
#include <LoopChainIR/LoopChain.hpp>
#include <LoopChainIR/Schedule.hpp>

enum SchedulePlan_T { SchedulePlan_t, ShiftPlan_t, FusePlan_t, SerialPlan_t, ParallelPlan_t, TilePlan_t, WavefrontPlan_t };

class SchedulePlan {
  public:
    SchedulePlan_T type;
    SchedulePlan( SchedulePlan_T type );
};

class SerialPlan : public SchedulePlan {
  public:
    SerialPlan();
};

class ParallelPlan : public SchedulePlan {
  public:
    ParallelPlan();
};

class ShiftPlan : public SchedulePlan {
  public:
    LoopChainIR::LoopChain::size_type loop_id;
    std::vector<std::string>* extents;
    ShiftPlan( LoopChainIR::LoopChain::size_type id, std::vector<std::string>* extents );
};

class FusePlan : public SchedulePlan {
  public:
    const bool auto_shift;
    FusePlan( bool auto_shift );
};

class WavefrontPlan : public SchedulePlan {
  public:
    WavefrontPlan();
};

class TilePlan : public SchedulePlan {
  public:
    std::vector<std::string>* extents;
    SchedulePlan* within_tiles;
    SchedulePlan* over_tiles;

  public:
    TilePlan( std::vector<std::string>* extents,SchedulePlan* over_tiles,  SchedulePlan* within_tiles );
};

class Scheduler {
  public:
    std::vector<SchedulePlan*>* plan_list;
    LoopChainIR::LoopChain* chain;
    std::vector<LoopChainIR::Transformation*> transformations;

    Scheduler( std::vector<SchedulePlan*>* plan_list, LoopChainIR::LoopChain* chain );

  private:
    bool pre_fuse;
    uint depth;
    LoopChainIR::LoopChain::size_type target_loop;

    std::vector<LoopChainIR::Transformation*> formTransformationFromPlan( SchedulePlan* plan );
    std::vector<LoopChainIR::Transformation*> formTransformationFromPlan( SerialPlan* plan );
    std::vector<LoopChainIR::Transformation*> formTransformationFromPlan( ParallelPlan* plan );
    std::vector<LoopChainIR::Transformation*> formTransformationFromPlan( ShiftPlan* plan );
    std::vector<LoopChainIR::Transformation*> formTransformationFromPlan( FusePlan* plan );
    std::vector<LoopChainIR::Transformation*> formTransformationFromPlan( WavefrontPlan* plan );
    std::vector<LoopChainIR::Transformation*> formTransformationFromPlan( TilePlan* plan );
};

#endif
