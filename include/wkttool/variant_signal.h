#pragma once
#include <boost/callable_traits/function_type.hpp>
#include <boost/signals2.hpp>
#include <wkttool/types.h>
// todo: add fold assert for having type
namespace wkttool
{
template <typename... Ts> struct InnerTypeWrapper
{
};

template <typename From, typename To, typename Enable = void>
struct CompatibleType
{
};

template <typename From, typename Head, typename... Tail>
struct CompatibleType<From, InnerTypeWrapper<Head, Tail...>,
                      std::enable_if_t<std::is_convertible_v<From, Head> > >
{
  using Type = Head;
};

template <typename From, typename Head, typename... Tail>
struct CompatibleType<From, InnerTypeWrapper<Head, Tail...>,
                      std::enable_if_t<!std::is_convertible_v<From, Head> > >
{
  using Type = typename CompatibleType<From, InnerTypeWrapper<Tail...> >::Type;
};
template <typename T> class VariantSignal
{
};

template <typename... Ts>
class VariantSignal<TypeList<Ts...> >
    : boost::signals2::signal<void (const Ts &)>...
{
public:
  template <typename T>
  boost::signals2::connection
  connect (T listener)
  {
    namespace ct = boost::callable_traits;
    return boost::signals2::signal<ct::function_type_t<T> >::connect (
        listener);
  }
  template <typename T>
  void
  operator() (const T &arg)
  {
    using Compatible =
        typename CompatibleType<T, InnerTypeWrapper<Ts...> >::Type;
    using SignalType = boost::signals2::signal<void (const Compatible &)>;
    SignalType::operator() (arg);
  }
};
}
