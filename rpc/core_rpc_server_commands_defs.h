// Copyright (c) 2014-2019, The Monero Project
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

#include "string_tools.h"

#include "cryptonote_protocol/cryptonote_protocol_defs.h"
#include "cryptonote_basic/cryptonote_basic.h"
#include "crypto/hash.h"
#include "common/varint.h"

namespace cryptonote
{
  //-----------------------------------------------
#define CORE_RPC_STATUS_OK   "OK"
#define CORE_RPC_STATUS_BUSY   "BUSY"
#define CORE_RPC_STATUS_NOT_MINING "NOT MINING"
#define CORE_RPC_STATUS_PAYMENT_REQUIRED "PAYMENT REQUIRED"

// When making *any* change here, bump minor
// If the change is incompatible, then bump major and set minor to 0
// This ensures CORE_RPC_VERSION always increases, that every change
// has its own version, and that clients can just test major to see
// whether they can talk to a given daemon without having to know in
// advance which version they will stop working with
// Don't go over 32767 for any of these
#define CORE_RPC_VERSION_MAJOR 3
#define CORE_RPC_VERSION_MINOR 0
#define MAKE_CORE_RPC_VERSION(major,minor) (((major)<<16)|(minor))
#define CORE_RPC_VERSION MAKE_CORE_RPC_VERSION(CORE_RPC_VERSION_MAJOR, CORE_RPC_VERSION_MINOR)

  struct rpc_request_base
  {
    BEGIN_KV_SERIALIZE_MAP()
    END_KV_SERIALIZE_MAP()
  };

  struct rpc_response_base
  {
    std::string status;
    bool untrusted;

    rpc_response_base(): untrusted(false) {}

    BEGIN_KV_SERIALIZE_MAP()
      KV_SERIALIZE(status)
      KV_SERIALIZE(untrusted)
    END_KV_SERIALIZE_MAP()
  };

  struct rpc_access_request_base: public rpc_request_base
  {
    std::string client;

    BEGIN_KV_SERIALIZE_MAP()
      KV_SERIALIZE_PARENT(rpc_request_base)
      KV_SERIALIZE(client)
    END_KV_SERIALIZE_MAP()
  };

  struct rpc_access_response_base: public rpc_response_base
  {
    uint64_t credits;
    std::string top_hash;

    rpc_access_response_base(): credits(0) {}

    BEGIN_KV_SERIALIZE_MAP()
      KV_SERIALIZE_PARENT(rpc_response_base)
      KV_SERIALIZE(credits)
      KV_SERIALIZE(top_hash)
    END_KV_SERIALIZE_MAP()
  };

  struct COMMAND_RPC_GET_BLOCKS_FAST
  {

    struct request_t: public rpc_access_request_base
    {
      std::list<crypto::hash> block_ids; //*first 10 blocks id goes sequential, next goes in pow(2,n) offset, like 2, 4, 8, 16, 32, 64 and so on, and the last one is always genesis block */
      uint64_t    start_height;
      bool        prune;
      bool        no_miner_tx;
      BEGIN_KV_SERIALIZE_MAP()
        KV_SERIALIZE_PARENT(rpc_access_request_base)
        KV_SERIALIZE_CONTAINER_POD_AS_BLOB(block_ids)
        KV_SERIALIZE(start_height)
        KV_SERIALIZE(prune)
        KV_SERIALIZE_OPT(no_miner_tx, false)
      END_KV_SERIALIZE_MAP()
    };
    typedef epee::misc_utils::struct_init<request_t> request;

    struct tx_output_indices
    {
      std::vector<uint64_t> indices;

      BEGIN_KV_SERIALIZE_MAP()
        KV_SERIALIZE(indices)
      END_KV_SERIALIZE_MAP()
    };

    struct block_output_indices
    {
      std::vector<tx_output_indices> indices;

      BEGIN_KV_SERIALIZE_MAP()
        KV_SERIALIZE(indices)
      END_KV_SERIALIZE_MAP()
    };

    struct response_t: public rpc_access_response_base
    {
      std::vector<block_complete_entry> blocks;
      uint64_t    start_height;
      uint64_t    current_height;
      std::vector<block_output_indices> output_indices;

      BEGIN_KV_SERIALIZE_MAP()
        KV_SERIALIZE_PARENT(rpc_access_response_base)
        KV_SERIALIZE(blocks)
        KV_SERIALIZE(start_height)
        KV_SERIALIZE(current_height)
        KV_SERIALIZE(output_indices)
      END_KV_SERIALIZE_MAP()
    };
    typedef epee::misc_utils::struct_init<response_t> response;
  };

  struct COMMAND_RPC_GET_TRANSACTION_POOL_HASHES_BIN
  {
    struct request_t: public rpc_access_request_base
    {
      BEGIN_KV_SERIALIZE_MAP()
        KV_SERIALIZE_PARENT(rpc_access_request_base)
      END_KV_SERIALIZE_MAP()
    };
    typedef epee::misc_utils::struct_init<request_t> request;

    struct response_t: public rpc_access_response_base
    {
      std::vector<crypto::hash> tx_hashes;

      BEGIN_KV_SERIALIZE_MAP()
        KV_SERIALIZE_PARENT(rpc_access_response_base)
        KV_SERIALIZE_CONTAINER_POD_AS_BLOB(tx_hashes)
      END_KV_SERIALIZE_MAP()
    };
    typedef epee::misc_utils::struct_init<response_t> response;
  };
}
