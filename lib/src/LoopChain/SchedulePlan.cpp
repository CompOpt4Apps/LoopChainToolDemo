#include "SchedulePlan.hpp"
#include "CHTmsg.H"

#include <LoopChainIR/FusionTransformation.hpp>
#include <LoopChainIR/ShiftTransformation.hpp>
#include <LoopChainIR/AutomaticShiftTransformation.hpp>
#include <LoopChainIR/TileTransformation.hpp>
#include <LoopChainIR/DefaultSequentialTransformation.hpp>
#include <LoopChainIR/WavefrontTransformation.hpp>
#include <LoopChainIR/ParallelAnnotation.hpp>

using namespace LoopChainIR;

SchedulePlan::SchedulePlan( SchedulePlan_T type ):type(type){}

SerialPlan::SerialPlan(): SchedulePlan( SerialPlan_t ){};

ParallelPlan::ParallelPlan(): SchedulePlan( ParallelPlan_t ){};

ShiftPlan::ShiftPlan( LoopChainIR::LoopChain::size_type id, std::vector<std::string>* extents ): SchedulePlan(ShiftPlan_t), loop_id(id), extents(extents){}

FusePlan::FusePlan( bool auto_shift ): SchedulePlan( FusePlan_t ), auto_shift( auto_shift ){};

WavefrontPlan::WavefrontPlan(): SchedulePlan( WavefrontPlan_t ){};

TilePlan::TilePlan( std::vector<std::string>* extents, SchedulePlan* over_tiles, SchedulePlan* within_tiles ): SchedulePlan(TilePlan_t), extents(extents), over_tiles(over_tiles), within_tiles(within_tiles){};

Scheduler::Scheduler( std::vector<SchedulePlan*>* plan_list, LoopChainIR::LoopChain* chain ): plan_list(plan_list), chain(chain), pre_fuse(true), depth(0), target_loop(0) {
  for( SchedulePlan* plan : *plan_list ){
    std::vector<LoopChainIR::Transformation*> transformation = this->formTransformationFromPlan( plan );
    this->transformations.insert( this->transformations.end(), transformation.begin(), transformation.end() );
  }
}

std::vector<LoopChainIR::Transformation*> Scheduler::formTransformationFromPlan( SchedulePlan* plan ){
  std::vector<LoopChainIR::Transformation*> transformation;

  switch( plan->type ){
    case SerialPlan_t:
      transformation = this->formTransformationFromPlan( static_cast<SerialPlan*>(plan) );
      break;

    case ParallelPlan_t:
      transformation = this->formTransformationFromPlan( static_cast<ParallelPlan*>(plan) );
      break;

    case ShiftPlan_t:
      transformation = this->formTransformationFromPlan( static_cast<ShiftPlan*>(plan) );
      break;

    case FusePlan_t:
      transformation = this->formTransformationFromPlan( static_cast<FusePlan*>(plan) );
      break;

    case WavefrontPlan_t:
      transformation = this->formTransformationFromPlan( static_cast<WavefrontPlan*>(plan) );
      break;

    case TilePlan_t:
      transformation = this->formTransformationFromPlan( static_cast<TilePlan*>(plan) );
      break;

    case SchedulePlan_t:
      assertWithException( false, "Cannot have abstract object SchedulePlan" );

    default:
      assertWithException( false, "Unknown SchedulePlan_T type encountered" );
  }

  return transformation;
}

std::vector<LoopChainIR::Transformation*> Scheduler::formTransformationFromPlan( SerialPlan* plan ){
  CHT::msg << CHT::fv4 << "SerialPlan" << CHT::end;
  std::vector<LoopChainIR::Transformation*> transformations;

  transformations.push_back( new DefaultSequentialTransformation() );

  return transformations;
}

std::vector<LoopChainIR::Transformation*> Scheduler::formTransformationFromPlan( ParallelPlan* plan ){
  CHT::msg << CHT::fv4 << "ParallelPlan" << CHT::end;
  std::vector<LoopChainIR::Transformation*> transformations;

  transformations.push_back( new ParallelAnnotation() );

  return transformations;
}

std::vector<LoopChainIR::Transformation*> Scheduler::formTransformationFromPlan( ShiftPlan* plan ){
  CHT::msg << CHT::fv4 << "ShiftPlan" << CHT::end;
  std::vector<LoopChainIR::Transformation*> transformations;

  transformations.push_back( new ShiftTransformation( plan->loop_id, *(plan->extents) ) );

  return transformations;
}

std::vector<LoopChainIR::Transformation*> Scheduler::formTransformationFromPlan( FusePlan* plan ){
  CHT::msg << CHT::fv4 << "FusePlan" << CHT::end;
  std::vector<LoopChainIR::Transformation*> transformations;

  CHT::msg << CHT::fv4 << "Auto shifting is " << (plan->auto_shift?"":"not ") << "enabled" << CHT::end;

  // if automatic shifts if required
  if( plan->auto_shift ){
    transformations.push_back( new AutomaticShiftTransformation() );
  }

  this->pre_fuse = false;

  std::vector<LoopChain::size_type> fuse_list;
  for( LoopChain::size_type i = 0; i < chain->length(); ++i ){
    fuse_list.push_back( i );
  }

  transformations.push_back( new FusionTransformation( fuse_list ) );

  return transformations;
}

std::vector<LoopChainIR::Transformation*> Scheduler::formTransformationFromPlan( WavefrontPlan* plan ){
  CHT::msg << CHT::fv4 << "WavefrontPlan" << CHT::end;
  std::vector<LoopChainIR::Transformation*> transformations;

  transformations.push_back( new WavefrontTransformation() );

  return transformations;
}

std::vector<LoopChainIR::Transformation*> Scheduler::formTransformationFromPlan( TilePlan* plan ){
  CHT::msg << CHT::fv4 << "TilePlan" << CHT::end;
  std::vector<LoopChainIR::Transformation*> transformations;

  LoopChain::size_type count = (this->pre_fuse && this->depth < 1 )? chain->length() : 1;
  for( LoopChain::size_type i = 0; i < count; ++i ){
    if( this->depth < 1 ){
      this->target_loop = i;
    }

    this->depth += 1;
    CHT::msg << CHT::fv4 << "Over Tiles" << CHT::end;
    std::vector<LoopChainIR::Transformation*> over_trans_list = formTransformationFromPlan( plan->over_tiles );
    CHT::msg << CHT::fv4 << "Within Tiles" << CHT::end;
    std::vector<LoopChainIR::Transformation*> within_trans_list = formTransformationFromPlan( plan->within_tiles );
    this->depth -= 1;

    CHT::msg << CHT::fv4 << over_trans_list.size() << ", " << within_trans_list.size() << CHT::end;

    assertWithException( over_trans_list.size() == 1, "Scheduler did not form exactly 1 plan for over tiles plan" );
    assertWithException( within_trans_list.size() == 1, "Scheduler did not form exactly 1 plan for within tiles plan" );

    TileTransformation::TileMap extents;
    TileTransformation::key_type dim = 0;
    for( std::string extent : *(plan->extents) ){
      extents[dim] = extent;
      dim += 1;
    }

    transformations.push_back( new TileTransformation( this->target_loop , extents, over_trans_list[0], within_trans_list[0]) );
  }

  return transformations;
}
