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

#include <config.h>
#include <time.h>
#include "uniform_reservoir.h"
#include "sampler.h"
#include <stdio.h>
#include <algorithm>
#include "static_vars.h"

using std::min;

namespace tcmalloc {

void UniformReservoir::Init() {
  rnd_ = time(NULL);
}

void UniformReservoir::update(size_t value) {
  const int c = ++count_;
  if (c < kSize) {
    values_[c] = value;
  } else {
    const size_t r = random(kSize*2);
    if (r < kSize) {
      values_[r] = value;
    }
  }
}

size_t UniformReservoir::random(size_t max) {
  long bits, val;

  do {
    bits = Sampler::NextRandom(rnd_) & (~(1L << sizeof(size_t)));
    val = bits % max;
  } while (bits - val + (max - 1) < 0L);

  return val;
}

void UniformReservoir::print() {
  size_t sorted_[kSize];
  size_t threshold = static_cast<size_t>(kSize * kLargeClassThreshold);
  struct aggregate aggregated_[static_cast<size_t>(kSize / threshold)];

  size_t val, count;
  unsigned int aggpointer = 0;

  size_t total = (min)(count_, (size_t)kSize);

  memcpy(&sorted_, &values_, kSize * sizeof(size_t));
  std::sort(sorted_, sorted_ + kSize);

  count = 0;
  val = sorted_[0];

  for(int i = 0; i < total; i++) {
    if (sorted_[i] == val) {
      count++;

      if (i == (total - 1) && count > threshold) {
	aggregated_[aggpointer].value = val;
	aggregated_[aggpointer].count = count;
	aggpointer++;
      }
    } else {
      if (count > threshold) {
	aggregated_[aggpointer].value = val;
	aggregated_[aggpointer].count = count;
	aggpointer++;
      }

      val = sorted_[i];
      count = 0;
      i--;
    }
  }

  for (int i = 0; i < aggpointer; i++) {
    Static::sizemap()->AddLargeSizeClass(aggregated_[i].value);
  }
}


} // namespace tcmalloc
