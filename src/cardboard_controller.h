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

#ifndef CARDBOARD_CONTROLLER_H_
#define CARDBOARD_CONTROLLER_H_

#include "controller.h"
#include "fplbase/input.h"
#include "game_state.h"
#include "precompiled.h"

namespace fpl {
namespace pie_noon {

class CardboardController : public Controller {
 public:
  CardboardController();

  void Initialize(GameState* game_state, fplbase::InputSystem* input_system);

  virtual void AdvanceFrame(WorldTime delta_time);

 private:
  GameState* game_state_;  // Pointer to the gamestate object
  // A pointer to the object to query for the current input state.
  fplbase::InputSystem* input_system_;
};

}  // pie_noon
}  // fpl

#endif  // CARDBOARD_CONTROLLER_H_
