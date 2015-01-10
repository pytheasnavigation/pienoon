// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string>
#include <sstream>
#include <vector>
#include "bulk_spline_evaluator.h"
#include "dual_cubic.h"

using mathfu::Lerp;

namespace fpl {

void BulkSplineEvaluator::SetNumIndices(const Index num_indices) {
  cubics_.resize(num_indices);
  domains_.resize(num_indices);
  sources_.resize(num_indices);
  results_.resize(num_indices);
}

void BulkSplineEvaluator::MoveIndex(const Index old_index,
                                    const Index new_index) {
  cubics_[new_index] = cubics_[old_index];
  domains_[new_index] = domains_[old_index];
  sources_[new_index] = sources_[old_index];
  results_[new_index] = results_[old_index];
}

void BulkSplineEvaluator::SetSpline(const Index index,
                                    const CompactSpline& spline,
                                    const float start_x) {
  domains_[index].x = start_x;
  sources_[index].spline = &spline;
  sources_[index].x_index = static_cast<CompactSplineIndex>(-1);
  InitCubic(index);
  EvaluateIndex(index);
}

void BulkSplineEvaluator::EvaluateIndex(const Index index) {
  const float cubic_x = CubicX(index);
  Result& r = results_[index];
  r.y = cubics_[index].Evaluate(cubic_x);
  r.derivative = cubics_[index].Derivative(cubic_x);
}

void BulkSplineEvaluator::AdvanceFrame(const float delta_x) {
  const Index num_splines = NumIndices();

  // Update x. We traverse the 'domains_' array linearly, which helps with
  // cache performance. TODO OPT: Add cache prefetching.
  for (Index index = 0; index < num_splines; ++index) {
    Domain& d = domains_[index];
    d.x += delta_x;

    // If x is out of the current range, reinitialize the cubic.
    if (!d.range.Contains(d.x)) {
      InitCubic(index);
    }
    d.x = d.range.Clamp(d.x);
  }

  // Evaluate the cubics. We traverse the 'cubics_' array linearly, which helps
  // with cache performance. TODO OPT: Add cache prefetching.
  for (Index index = 0; index < num_splines; ++index) {
    EvaluateIndex(index);
  }
}

bool BulkSplineEvaluator::Valid(const Index index) const {
  return 0 <= index && index < NumIndices() &&
         sources_[index].spline != nullptr;
}

void BulkSplineEvaluator::InitCubic(const Index index) {
  // Do nothing if the requested index has no spline.
  Domain& d = domains_[index];
  SourceData& s = sources_[index];
  if (s.spline == nullptr)
    return;

  // Do nothing if the current cubic matches the current spline segment.
  const CompactSplineIndex x_index = s.spline->IndexForX(d.x, s.x_index + 1);
  if (s.x_index == x_index)
    return;

  // Update the x-related values.
  s.x_index = x_index;
  d.range = s.spline->RangeX(x_index);

  // Initialize the cubic to interpolate the new spline segment.
  const CubicInit init = s.spline->CreateCubicInit(x_index);
  cubics_[index].Init(init);
}

} // namespace fpl