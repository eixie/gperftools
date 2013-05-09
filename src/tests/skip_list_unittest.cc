/* Copyright (c) 2006, Google Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---
 * Author: Sanjay Ghemawat
 */

#include <stdio.h>
#include "common.h"
#include "base/logging.h"
#include "src/skip_list.h"
#include "src/span.h"
#include "src/static_vars.h"

using tcmalloc::Span;
using tcmalloc::Static;
using tcmalloc::SkipList;
using tcmalloc::SkipList;

namespace {

static void TestSkipList() {
  Span* span = Static::span_allocator()->New();
  Span* span2 = Static::span_allocator()->New();
  Span* span3 = Static::span_allocator()->New();

  span->length = 1;
  span->start = 2;

  span2->length = 1;
  span2->start = 1;

  span3->length = 5;
  span3->start = 15;

  SkipList list;
  list.Init();
  list.Print();
  list.Insert(span3);
  list.Print();

  CHECK_EQ(NULL, list.GetBestFit(20));
  list.Print();
  CHECK_EQ(span3, list.GetBestFit(4));
  list.Print();
  CHECK_EQ(span2, list.GetBestFit(1));
  list.Print();
  // This check verifies that span2 has been removed from the list.
  CHECK_EQ(span, list.GetBestFit(1));
  list.Print();
  // This check verifies that span2 has been removed from the list.
  CHECK_EQ(NULL, list.GetBestFit(1));
  list.Insert(span2);
  CHECK_EQ(span2, list.GetBestFit(1));
}

static void TestSpanCompare() {
  Span* span = Static::span_allocator()->New();
  Span* span2 = Static::span_allocator()->New();

  // We use NULL to represent the head of the list, so we need to make sure that's
  // considered small.
  CHECK_EQ(-1, SpanCompare(NULL, span2));

  span->length = 1;
  span2->length = 2;

  CHECK_EQ(-1, SpanCompare(span, span2));

  span->length = 1;
  span2->length = 1;
  span->start = 1;
  span2->start = 2;

  CHECK_EQ(-1, SpanCompare(span, span2));

  span->length = 1;
  span2->length = 1;
  span->start = 2;
  span2->start = 1;

  CHECK_EQ(1, SpanCompare(span, span2));

  span->length = 1;
  span2->length = 1;
  span->start = 1;
  span2->start = 1;

  CHECK_EQ(0, SpanCompare(span, span2));
}

}

int main(int argc, char** argv) {
  TestSpanCompare();
  TestSkipList();

  printf("PASS\n");
  return 0;
}
