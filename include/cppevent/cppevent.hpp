#ifndef __SIGNAL_HEADER_GUARD__
#define __SIGNAL_HEADER_GUARD__

#include <iostream>
#include <mutex>
#include <functional>
#include <limits>
#include <algorithm>
#include <map>
#include <vector>

namespace cppevent
{
    struct integer_token
    {
        integer_token(int i) : token(i) {}
        friend bool operator<(const integer_token& lhs, const integer_token& rhs)
        {
            return lhs.token < rhs.token;
        }
    private:
        int token;
    };

    struct integer_token_generator
    {
        integer_token operator()() const
        {
            static int g_token = 0;
            return g_token++;
        }
    };

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
            return m_subscribers.emplace(token_generator(), func).first->first;
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
            friend Publisher; // Allows publisher to raise an event
        private:
            bool operator()(const Args&... args) override // denies from everyone except the publisher from raising an event
            {
                return signal_impl<Ret, Token, TokenGenerator, Args...>::operator()(args...);
            }
        };

    }

    /*
    A multicast delegate with arbitrary argument types

    Example:
    signal<void(int, std::string)> s;
    auto registration_token = (s += [](int i, std::string s) { });
    s(42, "Hello");
    s -= registration_token;
    */
    template <typename F, typename Token = integer_token, typename TokenGenerator = integer_token_generator>
    class signal : public helpers::_signal<F, Token, TokenGenerator>::type {};

    /*
    A multicast delegate with arbitrary argument types that allows only the hosting class to raise events

    Example:
    struct MyStruct
    {
    signal<void(int, std::string)> s;
    void raise()
    {
    s(42, "Hello");
    }
    };

    auto registration_token = (s += [](int i, std::string s) { });
    s(42, "Hello"); //Compilation error
    s -= registration_token;
    */
    template <typename Publisher, typename F, typename Token = integer_token, typename TokenGenerator = integer_token_generator>
    class event : public helpers::_event<Publisher, F, Token, TokenGenerator>
    {
        friend Publisher;
    };
}
#endif //__SIGNAL_HEADER_GUARD__