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
#include "common.h"
#include "skip_list.h"
#include "span.h"
#include "static_vars.h"

namespace tcmalloc {

SkipList::Node* SkipList::NewNode(Span* value) {
  Node* result = Static::skip_list_node_allocator()->New();
  memset(result, 0, sizeof(*result));
  result->value = value;
  return result;
}

void SkipList::DeleteNode(Node* node) {
  Static::skip_list_node_allocator()->Delete(node);
}

void SkipList::Init() {
  level_ = 0;
  head_ = NewNode(NULL);
}

void SkipList::Insert(Span* span) {
  Node* update[kSkipListHeight];
  Node* x = head_;

  for (int i = level_; i >= 0; i--) {
    while(x->forward[i] && SpanCompare(x->forward[i]->value, span) == -1) {
      x = x->forward[i];
    }
    update[i] = x;
  }

  unsigned int lvl = random_level();
  if (lvl > level_) {
    // increase the level of the list by a maximum of 1 as per the paper
    lvl = level_ + 1;
    level_ = lvl;
    update[lvl] = head_;
  }

  x = NewNode(span);
  for(int i = 0; i <= lvl; i++) {
    x->forward[i] = update[i]->forward[i];
    if (update[i]->forward[i])
      update[i]->forward[i]->backward[i] = x;

    update[i]->forward[i] = x;
    x->backward[i] = update[i];
  }
}

Span* SkipList::GetBestFit(size_t pages) {
  Node* x = head_;

  for (int i = level_; i >= 0; i--) {
    while(x->forward[i] &&
	    x->forward[i]->value &&
	      x->forward[i]->value->length < pages) {
      x = x->forward[i];
    }

    if (x->forward[0]) {
      x = x->forward[0];
      Span* rv = x->value;

      if (x->forward[i] == NULL && x->backward[i] == head_ && i == level_) {
	level_--;
      }

      for(int j = i; j >= 0; j--) {
	if (x->backward[j])
	  x->backward[j]->forward[j] = x->forward[j];
	if (x->forward[j])
	  x->forward[j]->backward[j] = x->backward[j];
      }

      DeleteNode(x);

      return rv;
    }
  }

  return NULL;
}

}
