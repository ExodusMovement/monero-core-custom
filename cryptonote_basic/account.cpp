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

#include <fstream>

#include "include_base_utils.h"
#include "account.h"
#include "warnings.h"
#include "crypto/crypto.h"
extern "C"
{
#include "crypto/keccak.h"
}
#include "cryptonote_basic_impl.h"
#include "cryptonote_format_utils.h"
#include "cryptonote_config.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "account"

using namespace std;

DISABLE_VS_WARNINGS(4244 4345)

  namespace cryptonote
{

  //-----------------------------------------------------------------
  hw::device& account_keys::get_device() const  {
    return *m_device;
  }
  //-----------------------------------------------------------------
  static void derive_key(const crypto::chacha_key &base_key, crypto::chacha_key &key)
  {
    static_assert(sizeof(base_key) == sizeof(crypto::hash), "chacha key and hash should be the same size");
    epee::mlocked<tools::scrubbed_arr<char, sizeof(base_key)+1>> data;
    memcpy(data.data(), &base_key, sizeof(base_key));
    data[sizeof(base_key)] = config::HASH_KEY_MEMORY;
    crypto::generate_chacha_key(data.data(), sizeof(data), key, 1);
  }
  //-----------------------------------------------------------------
  static epee::wipeable_string get_key_stream(const crypto::chacha_key &base_key, const crypto::chacha_iv &iv, size_t bytes)
  {
    // derive a new key
    crypto::chacha_key key;
    derive_key(base_key, key);

    // chacha
    epee::wipeable_string buffer0(std::string(bytes, '\0'));
    epee::wipeable_string buffer1 = buffer0;
    crypto::chacha20(buffer0.data(), buffer0.size(), key, iv, buffer1.data());
    return buffer1;
  }
  //-----------------------------------------------------------------
  void account_keys::xor_with_key_stream(const crypto::chacha_key &key)
  {
    // encrypt a large enough byte stream with chacha20
    epee::wipeable_string key_stream = get_key_stream(key, m_encryption_iv, sizeof(crypto::secret_key) * (2 + m_multisig_keys.size()));
    const char *ptr = key_stream.data();
    for (size_t i = 0; i < sizeof(crypto::secret_key); ++i)
      m_spend_secret_key.data[i] ^= *ptr++;
    for (size_t i = 0; i < sizeof(crypto::secret_key); ++i)
      m_view_secret_key.data[i] ^= *ptr++;
    for (crypto::secret_key &k: m_multisig_keys)
    {
      for (size_t i = 0; i < sizeof(crypto::secret_key); ++i)
        k.data[i] ^= *ptr++;
    }
  }
  //-----------------------------------------------------------------
  void account_keys::encrypt(const crypto::chacha_key &key)
  {
    m_encryption_iv = crypto::rand<crypto::chacha_iv>();
    xor_with_key_stream(key);
  }
  //-----------------------------------------------------------------
  void account_keys::decrypt(const crypto::chacha_key &key)
  {
    xor_with_key_stream(key);
  }
  //-----------------------------------------------------------------
  account_base::account_base()
  {
    set_null();
  }
  //-----------------------------------------------------------------
  void account_base::set_null()
  {
    m_keys = account_keys();
    m_creation_timestamp = 0;
  }
  //-----------------------------------------------------------------
  crypto::secret_key account_base::generate(const crypto::secret_key& recovery_key, bool recover, bool two_random)
  {
    crypto::secret_key first = generate_keys(m_keys.m_account_address.m_spend_public_key, m_keys.m_spend_secret_key, recovery_key, recover);

    // rng for generating second set of keys is hash of first rng.  means only one set of electrum-style words needed for recovery
    crypto::secret_key second;
    keccak((uint8_t *)&m_keys.m_spend_secret_key, sizeof(crypto::secret_key), (uint8_t *)&second, sizeof(crypto::secret_key));

    generate_keys(m_keys.m_account_address.m_view_public_key, m_keys.m_view_secret_key, second, two_random ? false : true);

    struct tm timestamp = {0};
    timestamp.tm_year = 2014 - 1900;  // year 2014
    timestamp.tm_mon = 6 - 1;  // month june
    timestamp.tm_mday = 8;  // 8th of june
    timestamp.tm_hour = 0;
    timestamp.tm_min = 0;
    timestamp.tm_sec = 0;

    if (recover)
    {
      m_creation_timestamp = mktime(&timestamp);
      if (m_creation_timestamp == (uint64_t)-1) // failure
        m_creation_timestamp = 0; // lowest value
    }
    else
    {
      m_creation_timestamp = time(NULL);
    }
    return first;
  }

  //-----------------------------------------------------------------
  const account_keys& account_base::get_keys() const
  {
    return m_keys;
  }
  //-----------------------------------------------------------------
  std::string account_base::get_public_address_str(network_type nettype) const
  {
    //TODO: change this code into base 58
    return get_account_address_as_str(nettype, false, m_keys.m_account_address);
  }
}
