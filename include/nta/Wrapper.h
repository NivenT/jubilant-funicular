#ifndef NTA_WRAPPER_H_INCLUDED
#define NTA_WRAPPER_H_INCLUDED

#include "nta/MyEngine.h"

#define NTA_OP_CHECKER(name, op) \
  template<typename T, typename U> Nop operator op(const T&, const U&); \
  template<typename T, typename U=T> \
  struct name##Exists { \
    enum { value = !::std::is_same<decltype(*(T*)(0) op *(U*)(0)), Nop>::value }; \
  };

#define NTA_UNARY_OP_CHECKER(name, op) \
  template<typename T> Nop operator op(T&); \
  template<typename T> \
  struct name##Exists { \
    enum { value = !::std::is_same<decltype(op *(T*)(0)), Nop>::value }; \
  };

#define NTA_IMPL_WRAPPER_OP(wrapper, type, name, op)	\
  template<typename T, typename U>							\
  static auto __##name(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<::nta::check:: name##Exists<T>::value, wrapper>{}) { \
    return lhs.m_data op rhs.m_data;					\
  } \
  template<typename T, typename U>							\
  static auto __##name(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<!::nta::check:: name##Exists<T>::value, wrapper>{}) { \
    return wrapper();							\
  } \
  wrapper operator op(const wrapper& rhs) const {	\
    return __##name<type, wrapper>(*this, rhs);		\
  }							\
  wrapper& operator op##=(const wrapper& rhs) {		\
    *this = *this op rhs;				\
    return *this;					\
  }

#define NTA_IMPL_WRAPPER_OP2(wrapper, type, name, op, rhs_type)  \
  template<typename T, typename U, typename V>              \
  static auto __##name(const U& lhs, const V& rhs) -> decltype(::std::enable_if_t<::nta::check:: name##Exists<T, rhs_type>::value, wrapper>{}) { \
    return lhs.m_data op rhs;          \
  } \
  template<typename T, typename U, typename V>              \
  static auto __##name(const U& lhs, const V& rhs) -> decltype(::std::enable_if_t<!::nta::check:: name##Exists<T, rhs_type>::value, wrapper>{}) { \
    return wrapper();             \
  } \
  wrapper operator op(const rhs_type& rhs) const { \
    return __##name<type, wrapper, rhs_type>(*this, rhs);   \
  }             \
  wrapper& operator op##=(const rhs_type& rhs) {   \
    *this = *this op rhs;       \
    return *this;         \
  }

#define NTA_IMPL_WRAPPER_COMP(wrapper, type, name, op)  \
  template<typename T, typename U>              \
  static auto __##name(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<::nta::check:: name##Exists<T>::value, bool>{}) { \
    return lhs.m_data op rhs.m_data;          \
  } \
  template<typename T, typename U>              \
  static auto __##name(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<!::nta::check:: name##Exists<T>::value, bool>{}) { \
    return wrapper();             \
  } \
  bool operator op(const wrapper& rhs) const { \
    return __##name<type, wrapper>(*this, rhs);   \
  }             \
  bool operator op##=(const wrapper& rhs) {   \
    return *this op rhs || *this == rhs; \
  }

// c should be "const" or "" (without the quotations)
#define NTA_IMPL_WRAPPER_UNARY_OP(wrapper, type, name, op, c)		\
  template<typename T, typename U>					\
  static auto __##name(c U& lhs) -> decltype(::std::enable_if_t<::nta::check:: name##Exists<T>::value, wrapper>{}) { \
    return op lhs.m_data;						\
  }									\
  template<typename T, typename U>					\
  static auto __##name(c U& lhs) -> decltype(::std::enable_if_t<!::nta::check:: name##Exists<T>::value, wrapper>{}) { \
    return wrapper();							\
  }									\
  wrapper operator op() c {						\
    return __##name<type, wrapper>(*this);				\
  }									\

/// \todo Overload shifts << and >>
#define NTA_CREATE_WRAPPER(name, type) \
  class name { \
  public: \
    typedef type inner; \
  private:     \
    type m_data;				\
  public:    \
    name(type data = type()) : m_data(data) {}	\
    type to_inner() const { return m_data; } \
    void __nta_wrapper() {} \
    template<typename T, typename U>              \
    static auto __Equals(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<::nta::check::EqualsExists<T>::value, bool>{}) { \
      return lhs.m_data == rhs.m_data;          \
    } \
    template<typename T, typename U>              \
    static auto __Equals(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<!::nta::check::EqualsExists<T>::value, bool>{}) { \
      return false;            \
    } \
    bool operator==(const name& rhs) const { \
      return __Equals<type, name>(*this, rhs);   \
    } \
    bool operator!=(const name& rhs) const { return !operator==(rhs); } \
    NTA_IMPL_WRAPPER_OP(name, type, Add, +)		\
    NTA_IMPL_WRAPPER_OP(name, type, Sub, -)		\
    NTA_IMPL_WRAPPER_UNARY_OP(name, type, Neg, -, const)	\
    NTA_IMPL_WRAPPER_OP(name, type, Div, /)		\
    NTA_IMPL_WRAPPER_OP(name, type, Mul, *)		\
    NTA_IMPL_WRAPPER_OP(name, type, And, &)		\
    NTA_IMPL_WRAPPER_OP(name, type, Or, |)		\
    NTA_IMPL_WRAPPER_UNARY_OP(name, type, Not, ~, const)	\
    NTA_IMPL_WRAPPER_UNARY_OP(name, type, Incr, ++,)		\
    name operator++(int) { auto ret = *this; operator++(); return ret; } \
    NTA_IMPL_WRAPPER_UNARY_OP(name, type, Decr, --,)		\
    name operator--(int) { auto ret = *this; operator--(); return ret; } \
    NTA_IMPL_WRAPPER_OP2(name, type, LShift, <<, int) \
    NTA_IMPL_WRAPPER_OP2(name, type, RShift, >>, int) \
    NTA_IMPL_WRAPPER_OP(name, type, Mod, %) \
    NTA_IMPL_WRAPPER_COMP(name, type, Greater, >) \
    NTA_IMPL_WRAPPER_COMP(name, type, Lesser, <) \
    template<typename T, typename U> \
    static auto __Print(::std::ostream& lhs, const U& rhs) -> decltype(::std::enable_if_t<::nta::check::LShiftExists<::std::ostream, T>::value, ::std::ostream*>{}) { \
      lhs<<#name<<"("<<rhs.m_data<<")";          \
      return &lhs; \
    } \
    template<typename T, typename U>              \
    static auto __Print(::std::ostream& lhs, const U& rhs) -> decltype(::std::enable_if_t<!::nta::check::LShiftExists<::std::ostream, T>::value, ::std::ostream*>{}) { \
      lhs<<#name;            \
      return &lhs; \
    } \
  };						\
  inline ::std::ostream& operator<<(::std::ostream& lhs, const name& rhs) { \
    return *name::__Print<type, name>(lhs, rhs);   \
  }
// The above function is inline to avoid the one-definition rule

/// Assumes type supports ::std::hash
///
/// Only use macro inside namespace std
#define NTA_HASH_WRAPPER(name) \
  template<> \
  struct hash<name> { \
    ::std::size_t operator()(const name& n) const { \
      return hash<name::inner>()(n.to_inner()); \
    } \
  };

/// \todo Make all indentation 2 spaces
namespace nta {
  /// Namespace for checking whether types support certain operations
  namespace check {
    struct Nop {};
    /// Struct for checking (at compile time) whether or not a class is a wrapper
    ///
    /// Usage: assert(is_wrapper<EntityID>::value == 1);
    template<typename T>
    struct is_wrapper { 
    private:
      typedef char one;
      struct two { char _[2]; };

      template<typename C> static one test(decltype(&C::__nta_wrapper));
      template<typename C> static two test(...);
    public:
      enum { value = sizeof(test<T>(0)) == sizeof(one) };
    };
    NTA_OP_CHECKER(Equals, ==)
    NTA_OP_CHECKER(Add, +)
    NTA_OP_CHECKER(Sub, -)
    NTA_UNARY_OP_CHECKER(Neg, -)
    NTA_OP_CHECKER(Div, /)
    NTA_OP_CHECKER(Mul, *)
    NTA_OP_CHECKER(And, &)
    NTA_OP_CHECKER(Or, |)
    NTA_UNARY_OP_CHECKER(Not, ~)
    NTA_UNARY_OP_CHECKER(Incr, ++)
    NTA_UNARY_OP_CHECKER(Decr, --) 
    NTA_OP_CHECKER(LShift, <<)
    NTA_OP_CHECKER(RShift, >>)
    NTA_OP_CHECKER(Mod, %)
    NTA_OP_CHECKER(Greater, >)
    NTA_OP_CHECKER(Lesser, <)
  }
}

#endif // NTA_WRAPPER_H_INCLUDED
