// Copyright (c) 2006-2013, Andrey N. Sabelnikov, www.sabelnikov.net
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Andrey N. Sabelnikov nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER  BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 



#ifndef _STRING_TOOLS_H_
#define _STRING_TOOLS_H_

// Previously pulled in by ASIO, further cleanup still required ...
#ifdef _WIN32
# include <winsock2.h>
# include <windows.h>
#endif

#include <string.h>
#include <locale>
#include <cstdlib>
#include <string>
#include <type_traits>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "hex.h"
#include "memwipe.h"
#include "mlocker.h"
#include "span.h"
#include "warnings.h"


#ifndef OUT
	#define OUT
#endif

#ifdef WINDOWS_PLATFORM
#pragma comment (lib, "Rpcrt4.lib")
#endif

namespace epee
{
namespace string_tools
{
  inline std::string buff_to_hex_nodelimer(const std::string& src)
  {
    return to_hex::string(to_byte_span(to_span(src)));
  }
  //----------------------------------------------------------------------------
  template<class CharT>
  bool parse_hexstr_to_binbuff(const std::basic_string<CharT>& s, std::basic_string<CharT>& res, bool allow_partial_byte = false)
  {
    res.clear();
    if (!allow_partial_byte && (s.size() & 1))
      return false;
    try
    {
      long v = 0;
      for(size_t i = 0; i < (s.size() + 1) / 2; i++)
      {
        CharT byte_str[3];
        size_t copied = s.copy(byte_str, 2, 2 * i);
        byte_str[copied] = CharT(0);
        CharT* endptr;
        v = strtoul(byte_str, &endptr, 16);
        if (v < 0 || 0xFF < v || endptr != byte_str + copied)
        {
          return false;
        }
        res.push_back(static_cast<unsigned char>(v));
      }

      return true;
    }catch(...)
    {
      return false;
    }
  }
	//----------------------------------------------------------------------------
	inline bool trim_left(std::string& str)
	{
		for(std::string::iterator it = str.begin(); it!= str.end() && isspace(static_cast<unsigned char>(*it));)
			str.erase(str.begin());
			
		return true;
	}
	//----------------------------------------------------------------------------
	inline bool trim_right(std::string& str)
	{

		for(std::string::reverse_iterator it = str.rbegin(); it!= str.rend() && isspace(static_cast<unsigned char>(*it));)
			str.erase( --((it++).base()));

		return true;
	}
	//----------------------------------------------------------------------------
	inline std::string& trim(std::string& str)
	{

		trim_left(str);
		trim_right(str);
		return str;
	}
  //----------------------------------------------------------------------------
  inline std::string trim(const std::string& str_)
  {
    std::string str = str_;
    trim_left(str);
    trim_right(str);
    return str;
  }
  //----------------------------------------------------------------------------
  template<class t_pod_type>
  std::string pod_to_hex(const t_pod_type& s)
  {
    static_assert(std::is_standard_layout<t_pod_type>(), "expected standard layout type");
    return to_hex::string(as_byte_span(s));
  }
  //----------------------------------------------------------------------------
  template<class t_pod_type>
  bool hex_to_pod(const std::string& hex_str, t_pod_type& s)
  {
    static_assert(std::is_pod<t_pod_type>::value, "expected pod type");
    std::string hex_str_tr = trim(hex_str);
    if(sizeof(s)*2 != hex_str.size())
      return false;
    std::string bin_buff;
    if(!parse_hexstr_to_binbuff(hex_str_tr, bin_buff))
      return false;
    if(bin_buff.size()!=sizeof(s))
      return false;

    s = *(t_pod_type*)bin_buff.data();
    return true;
  }
  //----------------------------------------------------------------------------
  template<class t_pod_type>
  bool hex_to_pod(const std::string& hex_str, tools::scrubbed<t_pod_type>& s)
  {
    return hex_to_pod(hex_str, unwrap(s));
  }
  //----------------------------------------------------------------------------
  template<class t_pod_type>
  bool hex_to_pod(const std::string& hex_str, epee::mlocked<t_pod_type>& s)
  {
    return hex_to_pod(hex_str, unwrap(s));
  }
  //----------------------------------------------------------------------------
  bool validate_hex(uint64_t length, const std::string& str);
}
}
#endif //_STRING_TOOLS_H_
