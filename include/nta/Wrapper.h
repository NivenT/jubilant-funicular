#ifndef NTA_WRAPPER_H_INCLUDED
#define NTA_WRAPPER_H_INCLUDED

#include <type_traits>

#define NTA_OP_CHECKER(name, op) \
  template<typename T, typename U> Nop operator op(const T&, const U&); \
  template<typename T, typename U=T> \
  struct name##Exists { \
    enum { value = !std::is_same<decltype(*(T*)(0) op *(U*)(0)), Nop>::value }; \
  };

#define NTA_UNARY_OP_CHECKER(name, op) \
  template<typename T> Nop operator op(const T&); \
  template<typename T> \
  struct name##Exists { \
    enum { value = !std::is_same<decltype(op *(T*)(0)), Nop>::value }; \
  };

#define NTA_IMPL_WRAPPER_OP(wrapper, type, name, op)	\
  template<typename T, typename U>							\
  static auto __##name(const U& lhs, const U& rhs) -> decltype(std::enable_if_t<check:: name##Exists<T>::value, wrapper>{}) { \
    return lhs.m_data op rhs.m_data;					\
  } \
  template<typename T, typename U>							\
  static auto __##name(const U& lhs, const U& rhs) -> decltype(std::enable_if_t<!check:: name##Exists<T>::value, wrapper>{}) { \
    return wrapper();							\
  } \
  wrapper operator op(const wrapper& rhs) const {	\
    return __##name<type, wrapper>(*this, rhs);		\
  }							\
  wrapper& operator op##=(const wrapper& rhs) {		\
    *this = *this op rhs;				\
    return *this;					\
  }


// c should be "const" or "" (without the quotations)
#define NTA_IMPL_WRAPPER_UNARY_OP(wrapper, type, name, op, c)		\
  template<typename T, typename U>					\
  static auto __##name(c U& lhs) -> decltype(std::enable_if_t<check:: name##Exists<T>::value, wrapper>{}) { \
    return op lhs.m_data;						\
  }									\
  template<typename T, typename U>					\
  static auto __##name(c U& lhs) -> decltype(std::enable_if_t<!check:: name##Exists<T>::value, wrapper>{}) { \
    return wrapper();							\
  }									\
  wrapper operator op() c {						\
    return __##name<type, wrapper>(*this);				\
  }									\

#define NTA_CREATE_WRAPPER(name, type) \
  class name { \
  private:     \
    type m_data;				\
  public:    \
    name(type data = type()) : m_data(data) {}	\
    operator type() { return m_data; } \
    bool operator==(const name& rhs) { 		\
      if constexpr (check::EqualsExists<type>::value) {	\
	  return m_data == rhs.m_data;			\
      } else {					\
	return false;					\
      } \
    }	\
    bool operator!=(const name& rhs) { return !operator==(rhs); } \
    NTA_IMPL_WRAPPER_OP(name, type, Add, +)		\
    NTA_IMPL_WRAPPER_OP(name, type, Sub, -)		\
    NTA_IMPL_WRAPPER_UNARY_OP(name, type, Neg, -, const)	\
    NTA_IMPL_WRAPPER_OP(name, type, Div, /)		\
    NTA_IMPL_WRAPPER_OP(name, type, Mul, *)		\
    NTA_IMPL_WRAPPER_OP(name, type, And, &)		\
    NTA_IMPL_WRAPPER_OP(name, type, Or, |)		\
    NTA_IMPL_WRAPPER_UNARY_OP(name, type, Not, ~, const)	\
    NTA_IMPL_WRAPPER_UNARY_OP(name, type, Incr, ++,)		\
    NTA_IMPL_WRAPPER_UNARY_OP(name, type, Decr, --,)		\
  };						\

/// \todo Make all indentation 2 spaces
namespace nta {
  /// Namespace for checking whether types support certain operations
  namespace check {
    struct Nop {};
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
  }
  //NTA_CREATE_WRAPPER(Test, int)
  //NTA_CREATE_WRAPPER(Test2, std::string)
}

#endif // NTA_WRAPPER_H_INCLUDED
