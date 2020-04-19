#ifndef NTA_EVENT_H_INCLUDED
#define NTA_EVENT_H_INCLUDED

#include <type_traits>

#include "nta/Option.h"

namespace nta {
    /// A template class for sending arbitrary type-safe Events
    ///
    /// See Event_tests.cpp for example usage
    template<typename RecipientEnum, typename... FuncTypes>
    class EventTemplate {
    static_assert(std::is_enum_v<RecipientEnum>,
            "EventTemplate: RecipientEnum must be enum");
    static_assert(std::size_t(RecipientEnum::COUNT) != 0,
            "EventTemplate: RecipientEnum must declare COUNT as the last variant");
    static_assert(sizeof...(FuncTypes) == std::size_t(RecipientEnum::COUNT),
            "EventTemplate: Every recipient must have a corresponding function signature");
    static_assert((std::is_function_v<FuncTypes> && ...),
            "EventTemplate: Every FuncType should be a function type");
    private:
        template<std::size_t idx, typename... Args>
        void enact_helper(Args&&... args) {
            if constexpr (std::is_invocable_v<decltype(std::get<idx>(m_actions).get()),
                                              Args...>) {
                auto func = std::get<idx>(m_actions);
                if (func) func.unwrap()(std::forward<Args>(args)...);
            }
        }
        template<std::size_t i, typename... Args>
        void operator_helper(std::size_t e, Args&&... args) {
            if (e == i) enact_helper<i>(std::forward<Args>(args)...);
            else if constexpr (i > 0) operator_helper<i-1>(e, 
                                                           std::forward<Args>(args)...);
        }
        template<std::size_t i, typename... Args>
        void operator2_helper(Args&&... args) {
            enact_helper<i>(std::forward<Args>(args)...);
            if constexpr (i > 0) operator2_helper<i-1>(std::forward<Args>(args)...);
        }

        std::tuple<utils::Option<std::function<FuncTypes>>...> m_actions;
    public:
        template<RecipientEnum e, typename Func>
        void define_for(Func&& func) {
            std::get<std::size_t(e)>(m_actions) = utils::make_some(func);
        }
        template<RecipientEnum e>
        void undefine_for() {
            std::get<std::size_t(e)>(m_actions).destroy();
        }

        template<RecipientEnum e, typename... Args>
        void enact(Args&&... args) {
            enact_helper<std::size_t(e)>(std::forward<Args>(args)...);
        }

        template<typename... Args>
        void operator()(RecipientEnum e, Args&&... args) {
            operator_helper<sizeof...(FuncTypes)-1>(std::size_t(e), 
                                                    std::forward<Args>(args)...);
        }
        template<typename... Args>
        void operator()(Args&&... args) {
            operator2_helper<sizeof...(FuncTypes)-1>(std::forward<Args>(args)...);
        }

        using enum_type = RecipientEnum;
    };
    /// Empty variant for use by ECS
    template<>
    class EventTemplate<void> {
    public:
        using enum_type = void;
    };
}

#endif // NTA_EVENT_H_INCLUDED
