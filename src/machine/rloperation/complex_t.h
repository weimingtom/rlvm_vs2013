// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2007 Elliot Glaysher
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
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
//
// -----------------------------------------------------------------------

#ifndef SRC_MACHINE_RLOPERATION_COMPLEX_T_H_
#define SRC_MACHINE_RLOPERATION_COMPLEX_T_H_

#include <string>
#include <tuple>
#include <vector>

#include "libreallive/bytecode_fwd.h"
#include "libreallive/expression.h"
#include "libreallive/expression_pieces.h"

// Type definition that implements the complex parameter concept.
//
// It really should have been called tuple, but the name's stuck
// now. Takes two other type structs as template parameters.
template <typename A, typename B>
struct Complex2_T {
  // The output type of this type struct
  typedef std::tuple<typename A::type, typename B::type> type;

  // Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine,
                      const libreallive::ExpressionPiecesVector& p,
                      unsigned int& position) {
    unsigned int pos_in_expression = 0;
    const libreallive::ComplexExpressionPiece& sp =
        static_cast<const libreallive::ComplexExpressionPiece&>(*p[position++]);
    typename A::type a =
        A::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename B::type b =
        B::getData(machine, sp.contained_pieces(), pos_in_expression);
    return type(a, b);
  }

  static void ParseParameters(unsigned int& position,
                              const std::vector<std::string>& input,
                              libreallive::ExpressionPiecesVector& output) {
	const unsigned char* data = (const unsigned char*)input.at(position).c_str();
    std::unique_ptr<libreallive::ExpressionPiece> ep(
        libreallive::GetComplexParam(data));
    output.push_back(std::move(ep));
    position++;
  }

  enum { is_complex = true };
};

// Type definition that implements the complex parameter concept.
//
// It really should have been called tuple, but the name's stuck
// now. Takes two other type structs as template parameters.
template <typename A, typename B, typename C>
struct Complex3_T {
  // The output type of this type struct
  typedef std::tuple<typename A::type, typename B::type, typename C::type> type;

  // Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine,
                      const libreallive::ExpressionPiecesVector& p,
                      unsigned int& position) {
    unsigned int pos_in_expression = 0;
    const libreallive::ComplexExpressionPiece& sp =
        static_cast<const libreallive::ComplexExpressionPiece&>(*p[position++]);
    typename A::type a =
        A::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename B::type b =
        B::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename C::type c =
        C::getData(machine, sp.contained_pieces(), pos_in_expression);
    return type(a, b, c);
  }

  static void ParseParameters(unsigned int& position,
                              const std::vector<std::string>& input,
                              libreallive::ExpressionPiecesVector& output) {
    const unsigned char* data = input.at(position).c_str();
    std::unique_ptr<libreallive::ExpressionPiece> ep(
        libreallive::GetComplexParam(data));
    output.push_back(std::move(ep));
    position++;
  }

  enum { is_complex = true };
};

// Type definition that implements the complex parameter concept.
//
// It really should have been called tuple, but the name's stuck
// now. Takes two other type structs as template parameters.
template <typename A, typename B, typename C, typename D>
struct Complex4_T {
  // The output type of this type struct
  typedef std::tuple<typename A::type,
                     typename B::type,
                     typename C::type,
                     typename D::type> type;

  // Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine,
                      const libreallive::ExpressionPiecesVector& p,
                      unsigned int& position) {
    unsigned int pos_in_expression = 0;

    const libreallive::ComplexExpressionPiece& sp =
        static_cast<const libreallive::ComplexExpressionPiece&>(*p[position++]);
    typename A::type a =
        A::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename B::type b =
        B::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename C::type c =
        C::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename D::type d =
        D::getData(machine, sp.contained_pieces(), pos_in_expression);
    return type(a, b, c, d);
  }

  static void ParseParameters(unsigned int& position,
                              const std::vector<std::string>& input,
                              libreallive::ExpressionPiecesVector& output) {
    const unsigned char* data = input.at(position).c_str();
    std::unique_ptr<libreallive::ExpressionPiece> ep(
        libreallive::GetComplexParam(data));
    output.push_back(std::move(ep));
    position++;
  }

  enum { is_complex = true };
};

// Type definition that implements the complex parameter concept.
//
// It really should have been called tuple, but the name's stuck
// now. Takes two other type structs as template parameters.
template <typename A,
          typename B,
          typename C,
          typename D,
          typename E,
          typename F,
          typename G>
struct Complex7_T {
  // The output type of this type struct
  typedef std::tuple<typename A::type,
                     typename B::type,
                     typename C::type,
                     typename D::type,
                     typename E::type,
                     typename F::type,
                     typename G::type> type;

  // Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine,
                      const libreallive::ExpressionPiecesVector& p,
                      unsigned int& position) {
    unsigned int pos_in_expression = 0;

    const libreallive::ComplexExpressionPiece& sp =
        static_cast<const libreallive::ComplexExpressionPiece&>(*p[position++]);
    typename A::type a =
        A::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename B::type b =
        B::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename C::type c =
        C::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename D::type d =
        D::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename E::type e =
        E::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename F::type f =
        F::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename G::type g =
        G::getData(machine, sp.contained_pieces(), pos_in_expression);
    return type(a, b, c, d, e, f, g);
  }

  static void ParseParameters(unsigned int& position,
                              const std::vector<std::string>& input,
                              libreallive::ExpressionPiecesVector& output) {
    const unsigned char* data = input.at(position).c_str();
    std::unique_ptr<libreallive::ExpressionPiece> ep(
        libreallive::GetComplexParam(data));
    output.push_back(std::move(ep));
    position++;
  }

  enum { is_complex = true };
};

// Type definition that implements the complex parameter concept.
//
// It really should have been called tuple, but the name's stuck
// now. Takes two other type structs as template parameters.
template <typename A,
          typename B,
          typename C,
          typename D,
          typename E,
          typename F,
          typename G,
          typename H>
struct Complex8_T {
  // The output type of this type struct
  typedef std::tuple<typename A::type,
                     typename B::type,
                     typename C::type,
                     typename D::type,
                     typename E::type,
                     typename F::type,
                     typename G::type,
                     typename H::type> type;

  // Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine,
                      const libreallive::ExpressionPiecesVector& p,
                      unsigned int& position) {
    unsigned int pos_in_expression = 0;
    const libreallive::ComplexExpressionPiece& sp =
        static_cast<const libreallive::ComplexExpressionPiece&>(*p[position++]);
    typename A::type a =
        A::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename B::type b =
        B::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename C::type c =
        C::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename D::type d =
        D::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename E::type e =
        E::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename F::type f =
        F::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename G::type g =
        G::getData(machine, sp.contained_pieces(), pos_in_expression);
    typename H::type h =
        H::getData(machine, sp.contained_pieces(), pos_in_expression);
    return type(a, b, c, d, e, f, g, h);
  }

  static void ParseParameters(unsigned int& position,
                              const std::vector<std::string>& input,
                              libreallive::ExpressionPiecesVector& output) {
    const unsigned char* data = input.at(position).c_str();
    std::unique_ptr<libreallive::ExpressionPiece> ep(
        libreallive::GetComplexParam(data));
    output.push_back(std::move(ep));
    position++;
  }

  enum { is_complex = true };
};

#endif  // SRC_MACHINE_RLOPERATION_COMPLEX_T_H_
