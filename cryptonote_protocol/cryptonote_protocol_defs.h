// Copyright (c) 2014-2020, The Monero Project
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#pragma once

#include <list>
#include "serialization/keyvalue_serialization.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "cryptonote_basic/blobdatatype.h"

namespace cryptonote
{

  /************************************************************************/
  /*                                                                      */
  /************************************************************************/
  struct tx_blob_entry
  {
    blobdata blob;
    crypto::hash prunable_hash;
    BEGIN_KV_SERIALIZE_MAP()
      KV_SERIALIZE(blob)
      KV_SERIALIZE_VAL_POD_AS_BLOB(prunable_hash)
    END_KV_SERIALIZE_MAP()

    tx_blob_entry(const blobdata &bd = {}, const crypto::hash &h = crypto::null_hash): blob(bd), prunable_hash(h) {}
  };
  struct block_complete_entry
  {
    bool pruned;
    blobdata block;
    uint64_t block_weight;
    std::vector<tx_blob_entry> txs;
    BEGIN_KV_SERIALIZE_MAP()
      KV_SERIALIZE_OPT(pruned, false)
      KV_SERIALIZE(block)
      KV_SERIALIZE_OPT(block_weight, (uint64_t)0)
      if (this_ref.pruned)
      {
        KV_SERIALIZE(txs)
      }
      else
      {
        std::vector<blobdata> txs;
        if (is_store)
        {
          txs.reserve(this_ref.txs.size());
          for (const auto &e: this_ref.txs) txs.push_back(e.blob);
        }
        epee::serialization::selector<is_store>::serialize(txs, stg, hparent_section, "txs");
        if (!is_store)
        {
          block_complete_entry &self = const_cast<block_complete_entry&>(this_ref);
          self.txs.clear();
          self.txs.reserve(txs.size());
          for (auto &e: txs) self.txs.push_back({std::move(e), crypto::null_hash});
        }
      }
    END_KV_SERIALIZE_MAP()

    block_complete_entry(): pruned(false), block_weight(0) {}
  };
}
