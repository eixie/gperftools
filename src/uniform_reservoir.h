// Copyright (c) 2008, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// ---
// Author: James Golick <jamesgolick@gmail.com>

#ifndef TCMALLOC_UNIFORM_RESERVOIR_H_
#define TCMALLOC_UNIFORM_RESERVOIR_H_

#include <config.h>
#include <stddef.h>                     // for size_t
#ifdef HAVE_STDINT_H
#include <stdint.h>                     // for uint64_t, int64_t, uint16_t
#endif
#include "common.h"

namespace tcmalloc {

class UniformReservoir {
 public:
  void Init();
  
  inline int size() {
    if (count_ > kSize) {
      return kSize;
    } else {
      return count_;
    }
  }

  inline int count() {
    return count_;
  }

  void update(size_t value);
  void print();

 private:
  static const int kSize = 1028;
  static const int kBitsPerSizeT = 64l;

  size_t rnd_;
  size_t count_;
  size_t values_[kSize];

  size_t random(size_t max);

  struct aggregate {
    size_t value;
    int count;
  };
};

}  // namespace tcmalloc

#endif  // TCMALLOC_UNIFORM_RESERVOIR_H_
