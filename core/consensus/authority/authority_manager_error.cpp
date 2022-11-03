/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "consensus/authority/authority_manager_error.hpp"

OUTCOME_CPP_DEFINE_CATEGORY(kagome::authority, AuthorityManagerError, e) {
  using E = kagome::authority::AuthorityManagerError;
  switch (e) {
    case E::UNKNOWN_ENGINE_ID:
      return "Unknown engine_id";
    case E::ORPHAN_BLOCK_OR_ALREADY_FINALIZED:
      return "Block is not descendant of last finalized block";
    case E::CAN_NOT_SAVE_STATE:
      return "Can not save state";
    case E::CANT_RECALCULATE_ON_PRUNED_STATE:
      return "Can't recalculate authority set ids on a pruned database";
   case E::FAILED_TO_INITIALIZE_SET_ID:
      return "Failed to initialize the current authority set id on startup";
  }
  return "unknown error (invalid AuthorityManagerError)";
}
