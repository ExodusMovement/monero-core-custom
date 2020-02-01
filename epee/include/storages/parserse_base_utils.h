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



#pragma once

#include <algorithm>

namespace epee
{
namespace misc_utils
{
  namespace parse
  {
    static const constexpr unsigned char isx[256] =
    {
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0,    1,    2,    3,    4,    5,    6,    7,    8,    9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff,   10,   11,   12,   13,   14,   15, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff,   10,   11,   12,   13,   14,   15, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };

    /*

      \b  Backspace (ascii code 08)
      \f  Form feed (ascii code 0C)
      \n  New line
      \r  Carriage return
      \t  Tab
      \v  Vertical tab
      \'  Apostrophe or single quote
      \"  Double quote
      \\  Backslash character

      */
      inline void match_string2(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val)
      {
        val.clear();
        val.reserve(std::distance(star_end_string, buf_end));
        bool escape_mode = false;
        std::string::const_iterator it = star_end_string;
        ++it;
        for(;it != buf_end;it++)
        {
          if(escape_mode/*prev_ch == '\\'*/)
          {
            switch(*it)
            {
            case 'b':  //Backspace (ascii code 08)
              val.push_back(0x08);break;
            case 'f':  //Form feed (ascii code 0C)
              val.push_back(0x0C);break;
            case 'n':  //New line
              val.push_back('\n');break;
            case 'r':  //Carriage return
              val.push_back('\r');break;
            case 't':  //Tab
              val.push_back('\t');break;
            case 'v':  //Vertical tab
              val.push_back('\v');break;
            case '\'':  //Apostrophe or single quote
              val.push_back('\'');break;
            case '"':  //Double quote
              val.push_back('"');break;
            case '\\':  //Backslash character
              val.push_back('\\');break;
            case '/':  //Slash character
              val.push_back('/');break;
            default:
              val.push_back(*it);
              LOG_PRINT_L0("Unknown escape sequence :\"\\" << *it << "\"");
            }
            escape_mode = false;
          }else if(*it == '"')
          {
            star_end_string = it;
            return;
          }else if(*it == '\\')
          {
            escape_mode = true;
          }
          else
          {
            val.push_back(*it);
          }
        }
        ASSERT_MES_AND_THROW("Failed to match string in json entry: " << std::string(star_end_string, buf_end));
      }
      inline void match_number2(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val, bool& is_float_val, bool& is_signed_val)
      {
        val.clear();
        is_float_val = false;
        for(std::string::const_iterator it = star_end_string;it != buf_end;it++)
        {
          if(isdigit(*it) || (it == star_end_string && *it == '-') || (val.size() && *it == '.' ) || (is_float_val && (*it == 'e' || *it == 'E' || *it == '-' || *it == '+' )) )
          {
            if(!val.size() &&  *it == '-')
              is_signed_val = true;
            if(*it == '.' )
              is_float_val = true;
            val.push_back(*it);
          }
          else
          {
            if(val.size())
            {
              star_end_string = --it;
              return;
            }
            else
              ASSERT_MES_AND_THROW("wrong number in json entry: " << std::string(star_end_string, buf_end));
          }
        }
        ASSERT_MES_AND_THROW("wrong number in json entry: " << std::string(star_end_string, buf_end));
      }
      inline void match_word2(std::string::const_iterator& star_end_string, std::string::const_iterator buf_end, std::string& val)
      {
        val.clear();

        for(std::string::const_iterator it = star_end_string;it != buf_end;it++)
        {
          if(!isalpha(*it))
          {
            val.assign(star_end_string, it);
            if(val.size())
            {
              star_end_string = --it;
              return;
            }else
              ASSERT_MES_AND_THROW("failed to match word number in json entry: " << std::string(star_end_string, buf_end));
          }
        }
        ASSERT_MES_AND_THROW("failed to match word number in json entry: " << std::string(star_end_string, buf_end));
      }
  }
}
}
