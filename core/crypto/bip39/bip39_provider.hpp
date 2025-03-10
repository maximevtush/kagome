/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/buffer.hpp"
#include "crypto/bip39/bip39_types.hpp"

namespace kagome::crypto {

  /**
   * @class Bip39Provider allows creating seed from mnemonic wordlist
   */
  class Bip39Provider {
   public:
    virtual ~Bip39Provider() = default;

    virtual std::string generatePhrase() const = 0;

    /**
     * @brief calculates entropy from mnemonic
     * @param correct mnemonic word list
     * @return entropy value
     */
    virtual outcome::result<std::vector<uint8_t>> calculateEntropy(
        const std::vector<std::string> &word_list) const = 0;

    /**
     * @brief makes seed from entropy
     * @param entropy entropy array
     * @return seed bytes
     */
    virtual outcome::result<bip39::Bip39Seed> makeSeed(
        common::BufferView entropy, std::string_view password) const = 0;

    virtual outcome::result<bip39::Bip39SeedAndJunctions> generateSeed(
        std::string_view mnemonic_phrase) const = 0;
  };

}  // namespace kagome::crypto
