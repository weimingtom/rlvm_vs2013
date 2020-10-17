// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 Elliot Glaysher
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

#ifndef SRC_SYSTEMS_BASE_TEXT_PAGE_H_
#define SRC_SYSTEMS_BASE_TEXT_PAGE_H_

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

class TextPageElement;
class SetWindowTextPageElement;
class System;
class TextTextPageElement;


// Represents the various commands.
enum CommandType {
  TYPE_CHARACTERS,
  TYPE_NAME,
  TYPE_KOE_MARKER,
  TYPE_HARD_BREAK,
  TYPE_SET_INDENTATION,
  TYPE_RESET_INDENTATION,
  TYPE_FONT_COLOUR,
  TYPE_DEFAULT_FONT_SIZE,
  TYPE_FONT_SIZE,
  TYPE_RUBY_BEGIN,
  TYPE_RUBY_END,
  TYPE_SET_INSERTION_X,
  TYPE_SET_INSERTION_Y,
  TYPE_OFFSET_INSERTION_X,
  TYPE_OFFSET_INSERTION_Y,
  TYPE_FACE_OPEN,
  TYPE_FACE_CLOSE,
  TYPE_NEXT_CHAR_IS_ITALIC,
};

// A sequence of replayable commands that write to or modify a window, such as
// displaying characters and changing font information.
//
// The majority of public methods in TextPage simply call the private versions
// of these methods, and add the appropriate TextPageElement to this page's
// back log for replay.
class TextPage {
 public:
  TextPage(System& system, int window_num);
#ifndef _MSC_VER
  TextPage(const TextPage& rhs);
  TextPage(TextPage&& rhs);
#endif
  ~TextPage();

  // Returns the number of characters printed with Character() and Name().
  int number_of_chars_on_page() const { return number_of_chars_on_page_; }

  // Queries to see if there has been an invocation of
  // MarkRubyBegin(), but not the closing DisplayRubyText().
  bool in_ruby_gloss() const { return in_ruby_gloss_; }

  bool empty() const { return elements_to_replay_.empty(); }

  // Replays every recordable action called on this TextPage.
  void Replay(bool is_active_page);

  // Add this character to the most recent text render operation on
  // this page's backlog, and then render it, minding the kinsoku
  // spacing rules.
  bool Character(const std::string& current, const std::string& rest);

  // Displays a name. This function will be called by the
  // TextoutLongOperation.
  void Name(const std::string& name, const std::string& next_char);

  // Puts a koe marker in the TextPage. This is only displayed during
  // scrollback.
  void KoeMarker(int id);

  // Forces a hard line brake.
  void HardBrake();

  // Sets the indentation to the x part of the current insertion point.
  void SetIndentation();

  // Resets the indentation.
  void ResetIndentation();

  // Sets the text foreground to the colour passed in, up until the
  // next pause().
  void FontColour(const int colour);

  // Changes the size of text.
  void DefaultFontSize();
  void FontSize(const int size);

  // Marks the current character as the beginning of a phrase that has
  // rubytext over it.
  void MarkRubyBegin();

  // Display the incoming phrase as the rubytext for the text since
  // MarkRubyBegin() was called.
  void DisplayRubyText(const std::string& utf8str);

  void SetInsertionPointX(int x);
  void SetInsertionPointY(int y);
  void Offset_insertion_point_x(int offset);
  void Offset_insertion_point_y(int offset);

  // Sets the face in slot |index| to filename.
  void FaceOpen(const std::string& filename, int index);

  // Removes the face in slot |index|.
  void FaceClose(int index);

  // Mark that the next character will be printed in italics (rlBabel
  // extension).
  void NextCharIsItalic();

  // Queries the corresponding TextWindow to see if it is full. Used
  // to implement implicit pauses when a page is full.
  bool IsFull() const;

 private:
  // Storage for an individual command.
  //struct Command;
// Storage for each command.
struct Command {
  explicit Command(CommandType type);
  Command(CommandType type, int one);
  Command(CommandType type, const std::string& one);
  Command(CommandType type, const std::string& one, const std::string& two);
  Command(CommandType type, const std::string& one, int two);
  Command(const Command& rhs);
  ~Command();

  enum CommandType command;
#ifndef _MSC_VER
  union {
#else
  struct {
#endif
    // TYPE_CHARACTERS
    std::string characters;

    // TYPE_NAME
    struct {
      std::string name;
      std::string next_char;
    } name;

    // TYPE_KOE_MARKER
    int koe_id;

    // TYPE_FONT_COLOUR
    int font_colour;

    // TYPE_FONT_SIZE
    int font_size;

    // TYPE_RUBY_END
    std::string ruby_text;

    // TYPE_SET_INSERTION_X
    int set_insertion_x;

    // TYPE_SET_INSERTION_Y
    int set_insertion_y;

    // TYPE_OFFSET_INSERTION_X
    int offset_insertion_x;

    // TYPE_OFFSET_INSERTION_Y
    int offset_insertion_y;

    // TYPE_FACE_OPEN
    struct {
      std::string filename;
      int index;
    } face_open;

    // TYPE_FACE_CLOSE
    int face_close;

    // For the empty types:
    // TYPE_HARD_BREAK
    // TYPE_SET_INDENTATION
    // TYPE_RESET_INDENTATION
    // TYPE_DEFAULT_FONT_SIZE
    // TYPE_RUBY_BEGIN
    int empty;
  };
};

  // Executes |command| and then adds it to |elements_to_replay_|.
  void AddAction(const Command& command);

  // Performs textout.
  bool CharacterImpl(const std::string& c, const std::string& rest);

  // Actually performs the command in most cases.
  void RunTextPageCommand(const Command& command,
                          bool is_active_page);

  System* system_;

  // Current window that this page is rendering into
  int window_num_;

  // Number of characters on this page (used in automode)
  int number_of_chars_on_page_;

  // Whether MarkRubyBegin() was called but DisplayRubyText() hasn't yet been
  // called.
  bool in_ruby_gloss_;

  // A list of the text elements to replay on this page.
  std::vector<Command> elements_to_replay_;
};

#endif  // SRC_SYSTEMS_BASE_TEXT_PAGE_H_
