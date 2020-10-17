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

#include "systems/base/object_mutator.h"

#include "machine/rlmachine.h"
#include "systems/base/event_system.h"
#include "systems/base/graphics_object.h"
#include "systems/base/graphics_object_data.h"
#include "systems/base/graphics_system.h"
#include "systems/base/parent_graphics_object_data.h"
#include "systems/base/system.h"
#include "utilities/math_util.h"

ObjectMutator::ObjectMutator(int repr,
                             const std::string& name,
                             int creation_time,
                             int duration_time,
                             int delay,
                             int type)
    : repr_(repr),
      name_(name),
      creation_time_(creation_time),
      duration_time_(duration_time),
      delay_(delay),
      type_(type) {
}

#ifndef _MSC_VER
ObjectMutator::ObjectMutator(const ObjectMutator& mutator) = default;
#endif

ObjectMutator::~ObjectMutator() {}

bool ObjectMutator::operator()(RLMachine& machine, GraphicsObject& object) {
  unsigned int ticks = machine.system().event().GetTicks();
  if (ticks > (creation_time_ + delay_)) {
    PerformSetting(machine, object);
    machine.system().graphics().mark_object_state_as_dirty();
  }
  return ticks > (creation_time_ + delay_ + duration_time_);
}

bool ObjectMutator::OperationMatches(int repr, const std::string& name) const {
  return repr_ == repr && name_ == name;
}

int ObjectMutator::GetValueForTime(RLMachine& machine, int start, int end) {
  unsigned int ticks = machine.system().event().GetTicks();
  if (ticks < (creation_time_ + delay_)) {
    return start;
  } else if (ticks < (creation_time_ + delay_ + duration_time_)) {
    return InterpolateBetween(creation_time_ + delay_,
                              ticks,
                              creation_time_ + delay_ + duration_time_,
                              start,
                              end,
                              type_);
  } else {
    return end;
  }
}

// -----------------------------------------------------------------------

OneIntObjectMutator::OneIntObjectMutator(const std::string& name,
                                         int creation_time,
                                         int duration_time,
                                         int delay,
                                         int type,
                                         int start_value,
                                         int target_value,
                                         Setter setter)
    : ObjectMutator(-1, name, creation_time, duration_time, delay, type),
      startval_(start_value),
      endval_(target_value),
      setter_(setter) {}

OneIntObjectMutator::~OneIntObjectMutator() {}

#ifndef _MSC_VER
OneIntObjectMutator::OneIntObjectMutator(const OneIntObjectMutator& rhs) =
  default;
#endif

void OneIntObjectMutator::SetToEnd(RLMachine& machine, GraphicsObject& object) {
  //(object.*setter_)(endval_);
	switch (setter_) {
	case GraphicsObject::func_SetX: object.SetX(endval_); break;
	case GraphicsObject::func_SetY: object.SetY(endval_); break;
	case GraphicsObject::func_SetAlpha: object.SetAlpha(endval_); break;
	case GraphicsObject::func_SetMono: object.SetMono(endval_); break;
	case GraphicsObject::func_SetInvert: object.SetInvert(endval_); break;
	case GraphicsObject::func_SetLight: object.SetLight(endval_); break;
	case GraphicsObject::func_SetTintRed: object.SetTintRed(endval_); break;
	case GraphicsObject::func_SetTintGreen: object.SetTintGreen(endval_); break;
	case GraphicsObject::func_SetTintBlue: object.SetTintBlue(endval_); break;
	case GraphicsObject::func_SetColourRed: object.SetColourRed(endval_); break;
	case GraphicsObject::func_SetColourGreen: object.SetColourGreen(endval_); break;
	case GraphicsObject::func_SetColourBlue: object.SetColourBlue(endval_); break;
	case GraphicsObject::func_SetColourLevel: object.SetColourLevel(endval_); break;
	case GraphicsObject::func_SetVert: object.SetVert(endval_); break;
	case GraphicsObject::func_SetWidth: object.SetWidth(endval_); break;
	case GraphicsObject::func_SetHeight: object.SetHeight(endval_); break;
	case GraphicsObject::func_SetRotation: object.SetRotation(endval_); break;
	case GraphicsObject::func_SetRepOriginX: object.SetRepOriginX(endval_); break;
	case GraphicsObject::func_SetRepOriginY: object.SetRepOriginY(endval_); break;
	case GraphicsObject::func_SetOriginX: object.SetOriginX(endval_); break;
	case GraphicsObject::func_SetOriginY: object.SetOriginY(endval_); break;
	case GraphicsObject::func_SetHqWidth: object.SetHqWidth(endval_); break;
	case GraphicsObject::func_SetHqHeight: object.SetHqHeight(endval_); break;
	case GraphicsObject::func_SetVisible: object.SetVisible(endval_); break;
	case GraphicsObject::func_SetCompositeMode: object.SetCompositeMode(endval_); break;
	case GraphicsObject::func_SetZLayer: object.SetZLayer(endval_); break;
	case GraphicsObject::func_SetZDepth: object.SetZDepth(endval_); break;
	case GraphicsObject::func_SetScrollRateX: object.SetScrollRateX(endval_); break;
	case GraphicsObject::func_SetScrollRateY: object.SetScrollRateY(endval_); break;
	case GraphicsObject::func_SetZOrder: object.SetZOrder(endval_); break;
	case GraphicsObject::func_SetPattNo: object.SetPattNo(endval_); break;
	case GraphicsObject::func_SetDigitValue: object.SetDigitValue(endval_); break;
	case GraphicsObject::func_SetButtonState: object.SetButtonState(endval_); break;
	default: throw;
	}
}

ObjectMutator* OneIntObjectMutator::Clone() const {
  return new OneIntObjectMutator(*this);
}

void OneIntObjectMutator::PerformSetting(RLMachine& machine,
                                         GraphicsObject& object) {
  int value = GetValueForTime(machine, startval_, endval_);
  //(object.*setter_)(value);
  switch (setter_) {
  case GraphicsObject::func_SetX: object.SetX(value); break;
  case GraphicsObject::func_SetY: object.SetY(value); break;
  case GraphicsObject::func_SetAlpha: object.SetAlpha(value); break;
  case GraphicsObject::func_SetMono: object.SetMono(value); break;
  case GraphicsObject::func_SetInvert: object.SetInvert(value); break;
  case GraphicsObject::func_SetLight: object.SetLight(value); break;
  case GraphicsObject::func_SetTintRed: object.SetTintRed(value); break;
  case GraphicsObject::func_SetTintGreen: object.SetTintGreen(value); break;
  case GraphicsObject::func_SetTintBlue: object.SetTintBlue(value); break;
  case GraphicsObject::func_SetColourRed: object.SetColourRed(value); break;
  case GraphicsObject::func_SetColourGreen: object.SetColourGreen(value); break;
  case GraphicsObject::func_SetColourBlue: object.SetColourBlue(value); break;
  case GraphicsObject::func_SetColourLevel: object.SetColourLevel(value); break;
  case GraphicsObject::func_SetVert: object.SetVert(value); break;
  case GraphicsObject::func_SetWidth: object.SetWidth(value); break;
  case GraphicsObject::func_SetHeight: object.SetHeight(value); break;
  case GraphicsObject::func_SetRotation: object.SetRotation(value); break;
  case GraphicsObject::func_SetRepOriginX: object.SetRepOriginX(value); break;
  case GraphicsObject::func_SetRepOriginY: object.SetRepOriginY(value); break;
  case GraphicsObject::func_SetOriginX: object.SetOriginX(value); break;
  case GraphicsObject::func_SetOriginY: object.SetOriginY(value); break;
  case GraphicsObject::func_SetHqWidth: object.SetHqWidth(value); break;
  case GraphicsObject::func_SetHqHeight: object.SetHqHeight(value); break;
  case GraphicsObject::func_SetVisible: object.SetVisible(value); break;
  case GraphicsObject::func_SetCompositeMode: object.SetCompositeMode(value); break;
  case GraphicsObject::func_SetZLayer: object.SetZLayer(value); break;
  case GraphicsObject::func_SetZDepth: object.SetZDepth(value); break;
  case GraphicsObject::func_SetScrollRateX: object.SetScrollRateX(value); break;
  case GraphicsObject::func_SetScrollRateY: object.SetScrollRateY(value); break;
  case GraphicsObject::func_SetZOrder: object.SetZOrder(value); break;
  case GraphicsObject::func_SetPattNo: object.SetPattNo(value); break;
  case GraphicsObject::func_SetDigitValue: object.SetDigitValue(value); break;
  case GraphicsObject::func_SetButtonState: object.SetButtonState(value); break;
  default: throw;
  }
}

// -----------------------------------------------------------------------

RepnoIntObjectMutator::RepnoIntObjectMutator(const std::string& name,
                                             int creation_time,
                                             int duration_time,
                                             int delay,
                                             int type,
                                             int repno,
                                             int start_value,
                                             int target_value,
                                             Setter setter)
    : ObjectMutator(repno, name, creation_time, duration_time, delay, type),
      repno_(repno),
      startval_(start_value),
      endval_(target_value),
      setter_(setter) {}

RepnoIntObjectMutator::~RepnoIntObjectMutator() {}

#ifndef _MSC_VER
RepnoIntObjectMutator::RepnoIntObjectMutator(const RepnoIntObjectMutator& rhs) =
  default;
#endif

void RepnoIntObjectMutator::SetToEnd(RLMachine& machine,
                                     GraphicsObject& object) {
  //(object.*setter_)(repno_, endval_);
	switch (setter_) {
	case GraphicsObject::func_SetXAdjustment: object.SetXAdjustment(repno_, endval_); break;
	case GraphicsObject::func_SetYAdjustment: object.SetYAdjustment(repno_, endval_); break;
	case GraphicsObject::func_SetAlphaAdjustment: object.SetAlphaAdjustment(repno_, endval_); break;
	default: throw;
	}
}

ObjectMutator* RepnoIntObjectMutator::Clone() const {
  return new RepnoIntObjectMutator(*this);
}

void RepnoIntObjectMutator::PerformSetting(RLMachine& machine,
                                           GraphicsObject& object) {
  int value = GetValueForTime(machine, startval_, endval_);
  //(object.*setter_)(repno_, value);
  switch (setter_) {
  case GraphicsObject::func_SetXAdjustment: object.SetXAdjustment(repno_, value); break;
  case GraphicsObject::func_SetYAdjustment: object.SetYAdjustment(repno_, value); break;
  case GraphicsObject::func_SetAlphaAdjustment: object.SetAlphaAdjustment(repno_, value); break;
  default: throw;
  }
}

// -----------------------------------------------------------------------

TwoIntObjectMutator::TwoIntObjectMutator(const std::string& name,
                                         int creation_time,
                                         int duration_time,
                                         int delay,
                                         int type,
                                         int start_one,
                                         int target_one,
                                         Setter setter_one,
                                         int start_two,
                                         int target_two,
                                         Setter setter_two)
    : ObjectMutator(-1, name, creation_time, duration_time, delay, type),
      startval_one_(start_one),
      endval_one_(target_one),
      setter_one_(setter_one),
      startval_two_(start_two),
      endval_two_(target_two),
      setter_two_(setter_two) {}

TwoIntObjectMutator::~TwoIntObjectMutator() {}

#ifndef _MSC_VER
TwoIntObjectMutator::TwoIntObjectMutator(const TwoIntObjectMutator& rhs) =
  default;
#endif

void TwoIntObjectMutator::SetToEnd(RLMachine& machine, GraphicsObject& object) {
  //(object.*setter_one_)(endval_one_);
	switch (setter_one_) {
	case GraphicsObject::func_SetX: object.SetX(endval_one_); break;
	case GraphicsObject::func_SetY: object.SetY(endval_one_); break;
	case GraphicsObject::func_SetAlpha: object.SetAlpha(endval_one_); break;
	case GraphicsObject::func_SetMono: object.SetMono(endval_one_); break;
	case GraphicsObject::func_SetInvert: object.SetInvert(endval_one_); break;
	case GraphicsObject::func_SetLight: object.SetLight(endval_one_); break;
	case GraphicsObject::func_SetTintRed: object.SetTintRed(endval_one_); break;
	case GraphicsObject::func_SetTintGreen: object.SetTintGreen(endval_one_); break;
	case GraphicsObject::func_SetTintBlue: object.SetTintBlue(endval_one_); break;
	case GraphicsObject::func_SetColourRed: object.SetColourRed(endval_one_); break;
	case GraphicsObject::func_SetColourGreen: object.SetColourGreen(endval_one_); break;
	case GraphicsObject::func_SetColourBlue: object.SetColourBlue(endval_one_); break;
	case GraphicsObject::func_SetColourLevel: object.SetColourLevel(endval_one_); break;
	case GraphicsObject::func_SetVert: object.SetVert(endval_one_); break;
	case GraphicsObject::func_SetWidth: object.SetWidth(endval_one_); break;
	case GraphicsObject::func_SetHeight: object.SetHeight(endval_one_); break;
	case GraphicsObject::func_SetRotation: object.SetRotation(endval_one_); break;
	case GraphicsObject::func_SetRepOriginX: object.SetRepOriginX(endval_one_); break;
	case GraphicsObject::func_SetRepOriginY: object.SetRepOriginY(endval_one_); break;
	case GraphicsObject::func_SetOriginX: object.SetOriginX(endval_one_); break;
	case GraphicsObject::func_SetOriginY: object.SetOriginY(endval_one_); break;
	case GraphicsObject::func_SetHqWidth: object.SetHqWidth(endval_one_); break;
	case GraphicsObject::func_SetHqHeight: object.SetHqHeight(endval_one_); break;
	case GraphicsObject::func_SetVisible: object.SetVisible(endval_one_); break;
	case GraphicsObject::func_SetCompositeMode: object.SetCompositeMode(endval_one_); break;
	case GraphicsObject::func_SetZLayer: object.SetZLayer(endval_one_); break;
	case GraphicsObject::func_SetZDepth: object.SetZDepth(endval_one_); break;
	case GraphicsObject::func_SetScrollRateX: object.SetScrollRateX(endval_one_); break;
	case GraphicsObject::func_SetScrollRateY: object.SetScrollRateY(endval_one_); break;
	case GraphicsObject::func_SetZOrder: object.SetZOrder(endval_one_); break;
	case GraphicsObject::func_SetPattNo: object.SetPattNo(endval_one_); break;
	case GraphicsObject::func_SetDigitValue: object.SetDigitValue(endval_one_); break;
	case GraphicsObject::func_SetButtonState: object.SetButtonState(endval_one_); break;
	default: throw;
	}

	
  //(object.*setter_two_)(endval_two_);
	switch (setter_two_) {
	case GraphicsObject::func_SetX: object.SetX(endval_two_); break;
	case GraphicsObject::func_SetY: object.SetY(endval_two_); break;
	case GraphicsObject::func_SetAlpha: object.SetAlpha(endval_two_); break;
	case GraphicsObject::func_SetMono: object.SetMono(endval_two_); break;
	case GraphicsObject::func_SetInvert: object.SetInvert(endval_two_); break;
	case GraphicsObject::func_SetLight: object.SetLight(endval_two_); break;
	case GraphicsObject::func_SetTintRed: object.SetTintRed(endval_two_); break;
	case GraphicsObject::func_SetTintGreen: object.SetTintGreen(endval_two_); break;
	case GraphicsObject::func_SetTintBlue: object.SetTintBlue(endval_two_); break;
	case GraphicsObject::func_SetColourRed: object.SetColourRed(endval_two_); break;
	case GraphicsObject::func_SetColourGreen: object.SetColourGreen(endval_two_); break;
	case GraphicsObject::func_SetColourBlue: object.SetColourBlue(endval_two_); break;
	case GraphicsObject::func_SetColourLevel: object.SetColourLevel(endval_two_); break;
	case GraphicsObject::func_SetVert: object.SetVert(endval_two_); break;
	case GraphicsObject::func_SetWidth: object.SetWidth(endval_two_); break;
	case GraphicsObject::func_SetHeight: object.SetHeight(endval_two_); break;
	case GraphicsObject::func_SetRotation: object.SetRotation(endval_two_); break;
	case GraphicsObject::func_SetRepOriginX: object.SetRepOriginX(endval_two_); break;
	case GraphicsObject::func_SetRepOriginY: object.SetRepOriginY(endval_two_); break;
	case GraphicsObject::func_SetOriginX: object.SetOriginX(endval_two_); break;
	case GraphicsObject::func_SetOriginY: object.SetOriginY(endval_two_); break;
	case GraphicsObject::func_SetHqWidth: object.SetHqWidth(endval_two_); break;
	case GraphicsObject::func_SetHqHeight: object.SetHqHeight(endval_two_); break;
	case GraphicsObject::func_SetVisible: object.SetVisible(endval_two_); break;
	case GraphicsObject::func_SetCompositeMode: object.SetCompositeMode(endval_two_); break;
	case GraphicsObject::func_SetZLayer: object.SetZLayer(endval_two_); break;
	case GraphicsObject::func_SetZDepth: object.SetZDepth(endval_two_); break;
	case GraphicsObject::func_SetScrollRateX: object.SetScrollRateX(endval_two_); break;
	case GraphicsObject::func_SetScrollRateY: object.SetScrollRateY(endval_two_); break;
	case GraphicsObject::func_SetZOrder: object.SetZOrder(endval_two_); break;
	case GraphicsObject::func_SetPattNo: object.SetPattNo(endval_two_); break;
	case GraphicsObject::func_SetDigitValue: object.SetDigitValue(endval_two_); break;
	case GraphicsObject::func_SetButtonState: object.SetButtonState(endval_two_); break;
	default: throw;
	}

}

ObjectMutator* TwoIntObjectMutator::Clone() const {
  return new TwoIntObjectMutator(*this);
}

void TwoIntObjectMutator::PerformSetting(RLMachine& machine,
                                         GraphicsObject& object) {
  int value = GetValueForTime(machine, startval_one_, endval_one_);
  //(object.*setter_one_)(value);
  switch (setter_one_) {
  case GraphicsObject::func_SetX: object.SetX(value); break;
  case GraphicsObject::func_SetY: object.SetY(value); break;
  case GraphicsObject::func_SetAlpha: object.SetAlpha(value); break;
  case GraphicsObject::func_SetMono: object.SetMono(value); break;
  case GraphicsObject::func_SetInvert: object.SetInvert(value); break;
  case GraphicsObject::func_SetLight: object.SetLight(value); break;
  case GraphicsObject::func_SetTintRed: object.SetTintRed(value); break;
  case GraphicsObject::func_SetTintGreen: object.SetTintGreen(value); break;
  case GraphicsObject::func_SetTintBlue: object.SetTintBlue(value); break;
  case GraphicsObject::func_SetColourRed: object.SetColourRed(value); break;
  case GraphicsObject::func_SetColourGreen: object.SetColourGreen(value); break;
  case GraphicsObject::func_SetColourBlue: object.SetColourBlue(value); break;
  case GraphicsObject::func_SetColourLevel: object.SetColourLevel(value); break;
  case GraphicsObject::func_SetVert: object.SetVert(value); break;
  case GraphicsObject::func_SetWidth: object.SetWidth(value); break;
  case GraphicsObject::func_SetHeight: object.SetHeight(value); break;
  case GraphicsObject::func_SetRotation: object.SetRotation(value); break;
  case GraphicsObject::func_SetRepOriginX: object.SetRepOriginX(value); break;
  case GraphicsObject::func_SetRepOriginY: object.SetRepOriginY(value); break;
  case GraphicsObject::func_SetOriginX: object.SetOriginX(value); break;
  case GraphicsObject::func_SetOriginY: object.SetOriginY(value); break;
  case GraphicsObject::func_SetHqWidth: object.SetHqWidth(value); break;
  case GraphicsObject::func_SetHqHeight: object.SetHqHeight(value); break;
  case GraphicsObject::func_SetVisible: object.SetVisible(value); break;
  case GraphicsObject::func_SetCompositeMode: object.SetCompositeMode(value); break;
  case GraphicsObject::func_SetZLayer: object.SetZLayer(value); break;
  case GraphicsObject::func_SetZDepth: object.SetZDepth(value); break;
  case GraphicsObject::func_SetScrollRateX: object.SetScrollRateX(value); break;
  case GraphicsObject::func_SetScrollRateY: object.SetScrollRateY(value); break;
  case GraphicsObject::func_SetZOrder: object.SetZOrder(value); break;
  case GraphicsObject::func_SetPattNo: object.SetPattNo(value); break;
  case GraphicsObject::func_SetDigitValue: object.SetDigitValue(value); break;
  case GraphicsObject::func_SetButtonState: object.SetButtonState(value); break;
  default: throw;
  }

  value = GetValueForTime(machine, startval_two_, endval_two_);
  //(object.*setter_two_)(value);
  switch (setter_two_) {
  case GraphicsObject::func_SetX: object.SetX(value); break;
  case GraphicsObject::func_SetY: object.SetY(value); break;
  case GraphicsObject::func_SetAlpha: object.SetAlpha(value); break;
  case GraphicsObject::func_SetMono: object.SetMono(value); break;
  case GraphicsObject::func_SetInvert: object.SetInvert(value); break;
  case GraphicsObject::func_SetLight: object.SetLight(value); break;
  case GraphicsObject::func_SetTintRed: object.SetTintRed(value); break;
  case GraphicsObject::func_SetTintGreen: object.SetTintGreen(value); break;
  case GraphicsObject::func_SetTintBlue: object.SetTintBlue(value); break;
  case GraphicsObject::func_SetColourRed: object.SetColourRed(value); break;
  case GraphicsObject::func_SetColourGreen: object.SetColourGreen(value); break;
  case GraphicsObject::func_SetColourBlue: object.SetColourBlue(value); break;
  case GraphicsObject::func_SetColourLevel: object.SetColourLevel(value); break;
  case GraphicsObject::func_SetVert: object.SetVert(value); break;
  case GraphicsObject::func_SetWidth: object.SetWidth(value); break;
  case GraphicsObject::func_SetHeight: object.SetHeight(value); break;
  case GraphicsObject::func_SetRotation: object.SetRotation(value); break;
  case GraphicsObject::func_SetRepOriginX: object.SetRepOriginX(value); break;
  case GraphicsObject::func_SetRepOriginY: object.SetRepOriginY(value); break;
  case GraphicsObject::func_SetOriginX: object.SetOriginX(value); break;
  case GraphicsObject::func_SetOriginY: object.SetOriginY(value); break;
  case GraphicsObject::func_SetHqWidth: object.SetHqWidth(value); break;
  case GraphicsObject::func_SetHqHeight: object.SetHqHeight(value); break;
  case GraphicsObject::func_SetVisible: object.SetVisible(value); break;
  case GraphicsObject::func_SetCompositeMode: object.SetCompositeMode(value); break;
  case GraphicsObject::func_SetZLayer: object.SetZLayer(value); break;
  case GraphicsObject::func_SetZDepth: object.SetZDepth(value); break;
  case GraphicsObject::func_SetScrollRateX: object.SetScrollRateX(value); break;
  case GraphicsObject::func_SetScrollRateY: object.SetScrollRateY(value); break;
  case GraphicsObject::func_SetZOrder: object.SetZOrder(value); break;
  case GraphicsObject::func_SetPattNo: object.SetPattNo(value); break;
  case GraphicsObject::func_SetDigitValue: object.SetDigitValue(value); break;
  case GraphicsObject::func_SetButtonState: object.SetButtonState(value); break;
  default: throw;
  }
}
