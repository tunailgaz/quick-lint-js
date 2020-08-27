// quick-lint-js finds bugs in JavaScript programs.
// Copyright (C) 2020  Matthew Glazar
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef QUICK_LINT_JS_PADDED_STRING_H
#define QUICK_LINT_JS_PADDED_STRING_H

#include <cassert>
#include <iosfwd>
#include <quick-lint-js/narrow-cast.h>
#include <string>

namespace quick_lint_js {
// Like std::string, but guaranteed to have several null bytes at the end.
//
// padded_string enables using SIMD instructions without extra bounds checking.
class padded_string {
 public:
  explicit padded_string(std::string &&string) : data_(std::move(string)) {
    int null_bytes_to_add = this->padding_size - 1;
    string.reserve(string.size() + narrow_cast<unsigned>(null_bytes_to_add));
    string.append(narrow_cast<unsigned>(null_bytes_to_add), '\0');
  }

  explicit padded_string(const char *string)
      : padded_string(std::string(string)) {}

  const char *c_str() const noexcept { return this->data_.c_str(); }

  int size() const noexcept { return narrow_cast<int>(this->data_.size()); }

  const char &operator[](int index) const noexcept {
    assert(index >= 0);
    assert(index <= this->size());
    return this->data_[narrow_cast<unsigned>(index)];
  }

  friend std::ostream &operator<<(std::ostream &, const padded_string &);

 private:
  static constexpr int padding_size = 8;

  std::string data_;
};

class padded_string_view {
 public:
  /*implicit*/ padded_string_view(const padded_string *string)
      : data_(string->c_str()) {}

  const char *c_str() const noexcept { return this->data_; }

 private:
  const char *data_;
};
}  // namespace quick_lint_js

#endif
