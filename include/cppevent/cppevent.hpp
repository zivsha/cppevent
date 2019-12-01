// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// Copyright (c) 2019 Ziv Shahaf <ziv.shahaf@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __CPPEVENT_HEADER_GUARD__
#define __CPPEVENT_HEADER_GUARD__

#include <mutex>        // for std::mutex
#include <functional>   // for std::function
#include <algorithm>    // for std::transform
#include <map>          // for std::map
#include <vector>       // for std::vector
#include <cstdint>      // for std::uintptr_t
#include <utility>      // for std::pair

namespace cppevent
{
    template<typename Ret, typename Token, typename TokenGenerator, typename... Args>
    class signal_impl
    {
        using FuncType = std::function<Ret(Args...)>;
    public:
        virtual ~signal_impl() = default;

        virtual Token operator+=(FuncType func)
        {
            return subscribe(std::move(func));
        }

        virtual bool operator-=(Token token)
        {
            return unsubscribe(std::move(token));
        }

        virtual bool operator()(const Args&... args)
        {
            return raise(args...);
        }

    private:

        Token subscribe(FuncType&& func)
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            return m_subscribers.emplace(token_generator(reinterpret_cast<std::uintptr_t>(this)), func).first->first;
        }

        bool unsubscribe(Token&& token)
        {
            std::lock_guard<std::mutex> locker(m_mutex);
            return m_subscribers.erase(token) > 0;
        }

        bool raise(const Args&... args)
        {
            std::vector<FuncType> registrations;

            {
                std::lock_guard<std::mutex> locker(m_mutex);
                std::transform(m_subscribers.begin(), m_subscribers.end(), std::back_inserter(registrations), [](const auto& kvp) {return kvp.second; });
            }

            if (registrations.empty())
                return false;

            for (auto func : registrations)
            {
                try
                {
                    func(args...);
                }
                catch (...)
                {
                    //...
                }
            }
            return true;

        }

        std::mutex m_mutex;
        std::map<Token, FuncType> m_subscribers;
        TokenGenerator token_generator;
    };

    namespace helpers
    {

        template<typename T, typename Token, typename TokenGenerator>
        struct _signal;

        template<typename Ret, typename Token, typename TokenGenerator, typename... Args>
        struct _signal<Ret(Args...), Token, TokenGenerator>
        {
            using type = signal_impl<Ret, Token, TokenGenerator, Args...>;
        };


        template<typename Publisher, typename T, typename Token, typename TokenGenerator>
        struct _event;

        template<typename Publisher, typename Ret, typename Token, typename TokenGenerator, typename... Args>
        struct _event<Publisher, Ret(Args...), Token, TokenGenerator> : public signal_impl<Ret, Token, TokenGenerator, Args...>
        {
            friend Publisher; // allows publisher to raise an event
        private:
            bool operator()(const Args&... args) override // denies from everyone except the publisher from raising an event
            {
                return signal_impl<Ret, Token, TokenGenerator, Args...>::operator()(args...);
            }
        };

    }

    struct simple_token_generator
    {
        struct simple_token
        {
            explicit simple_token(int i, std::uintptr_t id) : m_token(i), m_id(id) {}
            friend bool operator<(const simple_token& lhs, const simple_token& rhs)
            {
                return std::make_pair(lhs.m_token, lhs.m_id) < std::make_pair(rhs.m_token, rhs.m_id);
            }
        private:
            int m_token;
            std::uintptr_t m_id;
        };

        simple_token operator()(std::uintptr_t id)
        {
            return simple_token(token_count++, id);
        }
    private:
        int token_count = 0;
    };


    /*
        A multicast delegate with arbitrary argument types
    */
    template <typename F, typename Token = simple_token_generator::simple_token, typename TokenGenerator = simple_token_generator>
    class signal : public helpers::_signal<F, Token, TokenGenerator>::type {};

    /*
        A multicast delegate with arbitrary argument types that only allows the hosting class to raise events
    */
    template <typename Publisher, typename F, typename Token = simple_token_generator::simple_token, typename TokenGenerator = simple_token_generator>
    class event : public helpers::_event<Publisher, F, Token, TokenGenerator>
    {
        friend Publisher;
    };
}
#endif //__CPPEVENT_HEADER_GUARD__
