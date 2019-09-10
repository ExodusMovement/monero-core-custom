// Copyright (c) 2014-2018, MyMonero.com
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//  conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//  of conditions and the following disclaimer in the documentation and/or other
//  materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//  used to endorse or promote products derived from this software without specific
//  prior written permission.
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

#pragma once

#include <ostream>
#include <type_traits>

//! \return True iff if copying `T` requires only memmove ASM.
template<typename T>
constexpr bool is_cheap_copy() noexcept {
    return std::is_copy_constructible<T>() &&
        std::is_trivially_copyable<T>() &&
        std::is_trivially_destructible<T>();
}

//! \return True iff if there is no runtime penalty in passing `T` by value.
template<typename T>
constexpr bool is_by_value_cheap() noexcept {
    return is_cheap_copy<T>() && sizeof(T) <= sizeof(void*);
}

class logger {
public:
    /* String literals are converted to a pointer, and cheap to copy types are
       taken by value. Otherwise, the value is taken by const reference. */
    template<typename T>
    using decay = typename std::conditional<
        std::is_array<T>::value,
        const typename std::remove_extent<T>::type *,
        typename std::conditional<is_by_value_cheap<T>(), T, const T&>::type
    >::type;

    //! Associated level for the message and logger instance.
    enum level : unsigned char {
        kDebug = 0, //!< Maybe write to std::cerr (std::clog) with buffering
        kInfo,      //!< Maybe write to std::cerr (std::clog) with buffering
        kWarning,   //!< Maybe write to std::cerr (std::clog) with buffering
        kError      //<! Always write directly to std::cerr without buffering
    };

    //! Specific argument(s) for a log message (empty case)
    template<typename... T>
    class format {
    public:
        constexpr format() noexcept = default;
        constexpr format(const format&) noexcept = default;
        format& operator=(const format&) = delete;
        void write(std::ostream&) const noexcept {}
    };

    //! Specific argument(s) for a log message (1+ arguments case)
    template<typename Head, typename... Tail>
    class format<Head, Tail...> : protected format<Tail...> {
        static_assert(
            is_by_value_cheap<Head>() || std::is_lvalue_reference<Head>(),
            "invalid format setup"
        );

        const Head arg_;
    public:
        // Copies all format arguments
        constexpr format(const format<Tail...>& args, const Head& arg) noexcept
          : format<Tail...>(args), arg_(arg) {}

        constexpr format() noexcept = default;
        constexpr format(const format&) noexcept = default;
        format& operator=(const format&) = delete;

        void write(std::ostream& out) const {
        }
    };

    //! General information needed to log a message
    struct info {
        const char* const file_;
        const unsigned short line_;
        const level level_;
    };
    static_assert(
        sizeof(info) <= sizeof(void*) * 2,
        "unlikely to be passed via registers"
    );
    static_assert(is_cheap_copy<info>(), "info needs to be cheap to copy");
};

template<typename... T>
void operator&(const logger::info& info, const logger::format<T...>& args) {
}

template<typename... Tail, typename Head>
constexpr logger::format<logger::decay<Head>, Tail...>
operator<<(const logger::format<Tail...>& args, const Head& arg) noexcept {
    return {args, arg};
}

template<typename... Tail>
constexpr logger::format<std::ostream& (*)(std::ostream&), Tail...>
operator<<(const logger::format<Tail...>& args, std::ostream& (*arg)(std::ostream&) ) noexcept {
    return {args, arg};
}

#ifdef LOGGER_LOG
# error already defined
#endif
#define LOGGER_LOG(level) \
    logger::info{__FILE__, __LINE__, level} & logger::format<>{}

#ifdef LOGGER_ERROR
# error already defined
#endif
#define LOGGER_ERROR() LOGGER_LOG( logger::kError )

#ifdef LOGGER_WARNING
# error already defined
#endif
#define LOGGER_WARNING() LOGGER_LOG( logger::kWarning )

#ifdef LOGGER_INFO
# error already defined
#endif
#define LOGGER_INFO() LOGGER_LOG( logger::kInfo )

#ifdef LOGGER_DEBUG
# error already defined
#endif
#define LOGGER_DEBUG() LOGGER_LOG( logger::kDebug )
