#ifndef DEFER_H
#define DEFER_H
#include <type_traits>

#define DEFER(obj)          ::n19::Defer _(obj);
#define DEFER_IF(cond, obj) ::n19::Defer _([&](){ if((cond)){obj();} })

namespace n19 {
  template<typename T> requires std::is_invocable_v<T>
  class Defer {
    T obj_;
  public:
    Defer(const Defer&)             = delete;
    Defer& operator=(const Defer&)  = delete;
    Defer(const Defer&&)            = delete;
    Defer& operator=(const Defer&&) = delete;

    explicit Defer(T obj) : obj_(obj) {}
    ~Defer() { obj_(); }
  };
}

#endif //DEFER_H
