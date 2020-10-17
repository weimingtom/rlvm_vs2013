// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2013 Elliot Glaysher
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// -----------------------------------------------------------------------

#include "modules/object_mutator_operations.h"

#include "long_operations/wait_long_operation.h"
#include "machine/properties.h"
#include "machine/rlmachine.h"
#include "modules/module_obj.h"
#include "systems/base/event_system.h"
#include "systems/base/graphics_object.h"
#include "systems/base/graphics_system.h"
#include "systems/base/object_mutator.h"
#include "systems/base/system.h"

namespace {

bool MutatorIsDone(RLMachine& machine,
                   RLOperation* op,
                   int obj,
                   int repno,
                   const std::string& name) {
  return GetGraphicsObject(machine, op, obj)
             .IsMutatorRunningMatching(repno, name) == false;
}

bool ObjectMutatorIsWorking(RLMachine& machine,
                            RLOperation* op,
                            int obj,
                            int repno,
                            const std::string& name) {
  return GetGraphicsObject(machine, op, obj)
             .IsMutatorRunningMatching(repno, name) == false;
}

}  // namespace

// -----------------------------------------------------------------------

Op_ObjectMutatorInt::Op_ObjectMutatorInt(Getter getter,
                                         Setter setter,
                                         const std::string& name)
    : getter_(getter), setter_(setter), name_(name) {}

Op_ObjectMutatorInt::~Op_ObjectMutatorInt() {}

void Op_ObjectMutatorInt::operator()(RLMachine& machine,
                                     int object,
                                     int endval,
                                     int duration_time,
                                     int delay,
                                     int type) {
  unsigned int creation_time = machine.system().event().GetTicks();
  GraphicsObject& obj = GetGraphicsObject(machine, this, object);

  //int startval = (obj.*getter_)();
  int startval = 0;
  switch (getter_) {
  case GraphicsObject::func_x: startval = obj.x(); break;
  case GraphicsObject::func_y: startval = obj.y(); break;
  case GraphicsObject::func_raw_alpha: startval = obj.raw_alpha(); break;
  case GraphicsObject::func_mono: startval = obj.mono(); break;
  case GraphicsObject::func_invert: startval = obj.invert(); break;
  case GraphicsObject::func_light: startval = obj.light(); break;
  case GraphicsObject::func_tint_red: startval = obj.tint_red(); break;
  case GraphicsObject::func_tint_green: startval = obj.tint_green(); break;
  case GraphicsObject::func_tint_blue: startval = obj.tint_blue(); break;
  case GraphicsObject::func_colour_red: startval = obj.colour_red(); break;
  case GraphicsObject::func_colour_green: startval = obj.colour_green(); break;
  case GraphicsObject::func_colour_blue: startval = obj.colour_blue(); break;
  case GraphicsObject::func_colour_level: startval = obj.colour_level(); break;
  case GraphicsObject::func_vert: startval = obj.vert(); break;
  case GraphicsObject::func_width: startval = obj.width(); break;
  case GraphicsObject::func_height: startval = obj.height(); break;
  case GraphicsObject::func_rotation: startval = obj.rotation(); break;
  case GraphicsObject::func_rep_origin_x: startval = obj.rep_origin_x(); break;
  case GraphicsObject::func_rep_origin_y: startval = obj.rep_origin_y(); break;
  case GraphicsObject::func_origin_x: startval = obj.origin_x(); break;
  case GraphicsObject::func_origin_y: startval = obj.origin_y(); break;
  case GraphicsObject::func_hq_width: startval = obj.hq_width(); break;
  case GraphicsObject::func_hq_height: startval = obj.hq_height(); break;
  case GraphicsObject::func_visible: startval = obj.visible(); break;
  case GraphicsObject::func_composite_mode: startval = obj.composite_mode(); break;
  case GraphicsObject::func_z_layer: startval = obj.z_layer(); break;
  case GraphicsObject::func_z_depth: startval = obj.z_depth(); break;
  case GraphicsObject::func_scroll_rate_x: startval = obj.scroll_rate_x(); break;
  case GraphicsObject::func_scroll_rate_y: startval = obj.scroll_rate_y(); break;
  case GraphicsObject::func_z_order: startval = obj.z_order(); break;
  case GraphicsObject::func_GetPattNo: startval = obj.GetPattNo(); break;
  case GraphicsObject::func_GetDigitValue: startval = obj.GetDigitValue(); break;
  case GraphicsObject::func_GetButtonState: startval = obj.GetButtonState(); break;
  default: throw;
  }


  obj.AddObjectMutator(std::unique_ptr<ObjectMutator>(
      new OneIntObjectMutator(name_,
                              creation_time,
                              duration_time,
                              delay,
                              type,
                              startval,
                              endval,
                              setter_)));
}

// -----------------------------------------------------------------------

Op_ObjectMutatorRepnoInt::Op_ObjectMutatorRepnoInt(Getter getter,
                                                   Setter setter,
                                                   const std::string& name)
    : getter_(getter), setter_(setter), name_(name) {}

Op_ObjectMutatorRepnoInt::~Op_ObjectMutatorRepnoInt() {}

void Op_ObjectMutatorRepnoInt::operator()(RLMachine& machine,
                                          int object,
                                          int repno,
                                          int endval,
                                          int duration_time,
                                          int delay,
                                          int type) {
  unsigned int creation_time = machine.system().event().GetTicks();
  GraphicsObject& obj = GetGraphicsObject(machine, this, object);

  //int startval = (obj.*getter_)(repno);
  int startval = 0;
  switch (getter_) {
  case GraphicsObject::func_x_adjustment: startval = obj.x_adjustment(repno); break;
  case GraphicsObject::func_y_adjustment: startval = obj.y_adjustment(repno); break;
  case GraphicsObject::func_alpha_adjustment: startval = obj.alpha_adjustment(repno); break;
  default: throw;
  }

  obj.AddObjectMutator(std::unique_ptr<ObjectMutator>(
      new RepnoIntObjectMutator(name_,
                                creation_time,
                                duration_time,
                                delay,
                                type,
                                repno,
                                startval,
                                endval,
                                setter_)));
}

// -----------------------------------------------------------------------

Op_ObjectMutatorIntInt::Op_ObjectMutatorIntInt(Getter getter_one,
                                               Setter setter_one,
                                               Getter getter_two,
                                               Setter setter_two,
                                               const std::string& name)
    : getter_one_(getter_one),
      setter_one_(setter_one),
      getter_two_(getter_two),
      setter_two_(setter_two),
      name_(name) {}

Op_ObjectMutatorIntInt::~Op_ObjectMutatorIntInt() {}

void Op_ObjectMutatorIntInt::operator()(RLMachine& machine,
                                        int object,
                                        int endval_one,
                                        int endval_two,
                                        int duration_time,
                                        int delay,
                                        int type) {
  unsigned int creation_time = machine.system().event().GetTicks();
  GraphicsObject& obj = GetGraphicsObject(machine, this, object);
  //int startval_one = (obj.*getter_one_)();
  int startval_one = 0;
  switch (getter_one_) {
  case GraphicsObject::func_x: startval_one = obj.x(); break;
  case GraphicsObject::func_y: startval_one = obj.y(); break;
  case GraphicsObject::func_raw_alpha: startval_one = obj.raw_alpha(); break;
  case GraphicsObject::func_mono: startval_one = obj.mono(); break;
  case GraphicsObject::func_invert: startval_one = obj.invert(); break;
  case GraphicsObject::func_light: startval_one = obj.light(); break;
  case GraphicsObject::func_tint_red: startval_one = obj.tint_red(); break;
  case GraphicsObject::func_tint_green: startval_one = obj.tint_green(); break;
  case GraphicsObject::func_tint_blue: startval_one = obj.tint_blue(); break;
  case GraphicsObject::func_colour_red: startval_one = obj.colour_red(); break;
  case GraphicsObject::func_colour_green: startval_one = obj.colour_green(); break;
  case GraphicsObject::func_colour_blue: startval_one = obj.colour_blue(); break;
  case GraphicsObject::func_colour_level: startval_one = obj.colour_level(); break;
  case GraphicsObject::func_vert: startval_one = obj.vert(); break;
  case GraphicsObject::func_width: startval_one = obj.width(); break;
  case GraphicsObject::func_height: startval_one = obj.height(); break;
  case GraphicsObject::func_rotation: startval_one = obj.rotation(); break;
  case GraphicsObject::func_rep_origin_x: startval_one = obj.rep_origin_x(); break;
  case GraphicsObject::func_rep_origin_y: startval_one = obj.rep_origin_y(); break;
  case GraphicsObject::func_origin_x: startval_one = obj.origin_x(); break;
  case GraphicsObject::func_origin_y: startval_one = obj.origin_y(); break;
  case GraphicsObject::func_hq_width: startval_one = obj.hq_width(); break;
  case GraphicsObject::func_hq_height: startval_one = obj.hq_height(); break;
  case GraphicsObject::func_visible: startval_one = obj.visible(); break;
  case GraphicsObject::func_composite_mode: startval_one = obj.composite_mode(); break;
  case GraphicsObject::func_z_layer: startval_one = obj.z_layer(); break;
  case GraphicsObject::func_z_depth: startval_one = obj.z_depth(); break;
  case GraphicsObject::func_scroll_rate_x: startval_one = obj.scroll_rate_x(); break;
  case GraphicsObject::func_scroll_rate_y: startval_one = obj.scroll_rate_y(); break;
  case GraphicsObject::func_z_order: startval_one = obj.z_order(); break;
  case GraphicsObject::func_GetPattNo: startval_one = obj.GetPattNo(); break;
  case GraphicsObject::func_GetDigitValue: startval_one = obj.GetDigitValue(); break;
  case GraphicsObject::func_GetButtonState: startval_one = obj.GetButtonState(); break;
  default: throw;
  }


  
  //int startval_two = (obj.*getter_two_)();
  int startval_two = 0;
  switch (getter_two_) {
  case GraphicsObject::func_x: startval_two = obj.x(); break;
  case GraphicsObject::func_y: startval_two = obj.y(); break;
  case GraphicsObject::func_raw_alpha: startval_two = obj.raw_alpha(); break;
  case GraphicsObject::func_mono: startval_two = obj.mono(); break;
  case GraphicsObject::func_invert: startval_two = obj.invert(); break;
  case GraphicsObject::func_light: startval_two = obj.light(); break;
  case GraphicsObject::func_tint_red: startval_two = obj.tint_red(); break;
  case GraphicsObject::func_tint_green: startval_two = obj.tint_green(); break;
  case GraphicsObject::func_tint_blue: startval_two = obj.tint_blue(); break;
  case GraphicsObject::func_colour_red: startval_two = obj.colour_red(); break;
  case GraphicsObject::func_colour_green: startval_two = obj.colour_green(); break;
  case GraphicsObject::func_colour_blue: startval_two = obj.colour_blue(); break;
  case GraphicsObject::func_colour_level: startval_two = obj.colour_level(); break;
  case GraphicsObject::func_vert: startval_two = obj.vert(); break;
  case GraphicsObject::func_width: startval_two = obj.width(); break;
  case GraphicsObject::func_height: startval_two = obj.height(); break;
  case GraphicsObject::func_rotation: startval_two = obj.rotation(); break;
  case GraphicsObject::func_rep_origin_x: startval_two = obj.rep_origin_x(); break;
  case GraphicsObject::func_rep_origin_y: startval_two = obj.rep_origin_y(); break;
  case GraphicsObject::func_origin_x: startval_two = obj.origin_x(); break;
  case GraphicsObject::func_origin_y: startval_two = obj.origin_y(); break;
  case GraphicsObject::func_hq_width: startval_two = obj.hq_width(); break;
  case GraphicsObject::func_hq_height: startval_two = obj.hq_height(); break;
  case GraphicsObject::func_visible: startval_two = obj.visible(); break;
  case GraphicsObject::func_composite_mode: startval_two = obj.composite_mode(); break;
  case GraphicsObject::func_z_layer: startval_two = obj.z_layer(); break;
  case GraphicsObject::func_z_depth: startval_two = obj.z_depth(); break;
  case GraphicsObject::func_scroll_rate_x: startval_two = obj.scroll_rate_x(); break;
  case GraphicsObject::func_scroll_rate_y: startval_two = obj.scroll_rate_y(); break;
  case GraphicsObject::func_z_order: startval_two = obj.z_order(); break;
  case GraphicsObject::func_GetPattNo: startval_two = obj.GetPattNo(); break;
  case GraphicsObject::func_GetDigitValue: startval_two = obj.GetDigitValue(); break;
  case GraphicsObject::func_GetButtonState: startval_two = obj.GetButtonState(); break;
  default: throw;
  }

  obj.AddObjectMutator(std::unique_ptr<ObjectMutator>(
      new TwoIntObjectMutator(name_,
                              creation_time,
                              duration_time,
                              delay,
                              type,
                              startval_one,
                              endval_one,
                              setter_one_,
                              startval_two,
                              endval_two,
                              setter_two_)));
}

// -----------------------------------------------------------------------

Op_EndObjectMutation_Normal::Op_EndObjectMutation_Normal(
    const std::string& name)
    : name_(name) {}

Op_EndObjectMutation_Normal::~Op_EndObjectMutation_Normal() {}

void Op_EndObjectMutation_Normal::operator()(RLMachine& machine,
                                             int object,
                                             int speedup) {
  GetGraphicsObject(machine, this, object)
      .EndObjectMutatorMatching(machine, -1, name_, speedup);
}

// -----------------------------------------------------------------------

Op_EndObjectMutation_RepNo::Op_EndObjectMutation_RepNo(const std::string& name)
    : name_(name) {}

Op_EndObjectMutation_RepNo::~Op_EndObjectMutation_RepNo() {}

void Op_EndObjectMutation_RepNo::operator()(RLMachine& machine,
                                            int object,
                                            int repno,
                                            int speedup) {
  GetGraphicsObject(machine, this, object)
      .EndObjectMutatorMatching(machine, repno, name_, speedup);
}

// -----------------------------------------------------------------------

Op_MutatorCheck::Op_MutatorCheck(const std::string& name)
    : name_(name) {}

Op_MutatorCheck::~Op_MutatorCheck() {}

int Op_MutatorCheck::operator()(RLMachine& machine, int object) {
  GraphicsObject& obj = GetGraphicsObject(machine, this, object);
  return obj.IsMutatorRunningMatching(object, name_) ? 1 : 0;
}

// -----------------------------------------------------------------------

Op_MutatorWaitNormal::Op_MutatorWaitNormal(const std::string& name)
    : name_(name) {}

Op_MutatorWaitNormal::~Op_MutatorWaitNormal() {}

void Op_MutatorWaitNormal::operator()(RLMachine& machine, int obj) {
  WaitLongOperation* wait_op = new WaitLongOperation(machine);
  wait_op->BreakOnEvent(
      std::bind(MutatorIsDone, std::ref(machine), this, obj, -1, name_));
  machine.PushLongOperation(wait_op);
}

// -----------------------------------------------------------------------

Op_MutatorWaitRepNo::Op_MutatorWaitRepNo(const std::string& name)
    : name_(name) {}

Op_MutatorWaitRepNo::~Op_MutatorWaitRepNo() {}

void Op_MutatorWaitRepNo::operator()(RLMachine& machine, int obj, int repno) {
  WaitLongOperation* wait_op = new WaitLongOperation(machine);
  wait_op->BreakOnEvent(
      std::bind(MutatorIsDone, std::ref(machine), this, obj, repno, name_));
  machine.PushLongOperation(wait_op);
}

// -----------------------------------------------------------------------

Op_MutatorWaitCNormal::Op_MutatorWaitCNormal(const std::string& name)
    : name_(name) {}

Op_MutatorWaitCNormal::~Op_MutatorWaitCNormal() {}

void Op_MutatorWaitCNormal::operator()(RLMachine& machine, int obj) {
  WaitLongOperation* wait_op = new WaitLongOperation(machine);
  wait_op->BreakOnClicks();
  wait_op->BreakOnEvent(std::bind(
      ObjectMutatorIsWorking, std::ref(machine), this, obj, -1, name_));
  machine.PushLongOperation(wait_op);
}

// -----------------------------------------------------------------------

Op_MutatorWaitCRepNo::Op_MutatorWaitCRepNo(const std::string& name)
    : name_(name) {}

Op_MutatorWaitCRepNo::~Op_MutatorWaitCRepNo() {}

void Op_MutatorWaitCRepNo::operator()(RLMachine& machine, int obj, int repno) {
  WaitLongOperation* wait_op = new WaitLongOperation(machine);
  wait_op->BreakOnClicks();
  wait_op->BreakOnEvent(std::bind(
      ObjectMutatorIsWorking, std::ref(machine), this, obj, repno, name_));
  machine.PushLongOperation(wait_op);
}
