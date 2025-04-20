#include "boost/type_index.hpp"
#include <gtest/gtest.h>

// Unpack tuple with arguments and call function

template<typename Callback, typename Tuple, bool Enough, int TotalArgs, int... N>
struct call_impl {
    auto static call_w_tuple_args_impl(Callback cb, Tuple&& t) {
        return call_impl<Callback, Tuple, TotalArgs == 1 + sizeof...(N),

                                          TotalArgs, N..., sizeof...(N)
            >::call_w_tuple_args_impl(cb, std::forward<Tuple>(t));
    }
};

template<typename Callback, typename Tuple, int TotalArgs, int... N>
struct call_impl<Callback, Tuple, true, TotalArgs, N...> {
    auto static call_w_tuple_args_impl(Callback cb, Tuple&& t) {
        return cb(std::get<N>(std::forward<Tuple>(t))...);
    }
};

template<typename Callback, typename Tuple>
auto call_w_tuple_args(Callback cb, Tuple&& t) {
    typedef typename std::decay<Tuple>::type type;
    return call_impl<Callback, Tuple, 0 == std::tuple_size<type>::value,

                                           std::tuple_size<type>::value
                    >::call_w_tuple_args_impl(cb, std::forward<Tuple>(t));
}

// Count uncaught exceptions
class UncaughtExceptions {
public:
    UncaughtExceptions() : count_(std::uncaught_exceptions()) {}
    operator bool() { return count_ != std::uncaught_exceptions(); }
private:
    const int count_;
};

// Scope guard

template <bool on_success, bool on_failure, typename Callback, typename... Args>
class ScopedGuardImpl {
public:
    template <typename = void>
    ScopedGuardImpl(const Callback& cb, const Args&... args)
        : cb_(cb), args_(args...) {
        std::cout << "-- ScopedGuard: constructor (const Callback&, const Args&...) -> "
            << boost::typeindex::type_id_with_cvr<decltype(cb)>().pretty_name()
            << std::endl;
    }

    ~ScopedGuardImpl() {
        std::cout << "-- ScopedGuard: destructor" << std::endl;
        if ((on_success && !hasUncaughtExceptions_) ||
            (on_failure && hasUncaughtExceptions_)) {

            call_w_tuple_args(cb_, args_);

        }
    }

    ScopedGuardImpl(ScopedGuardImpl&& other)
        : cb_(std::move(other.cb_)),
          args_(std::move(other.args_)) {

        std::cout << "-- ScopedGuard: move" << std::endl;
    }

private:
    ScopedGuardImpl& operator=(const ScopedGuardImpl&) = delete;

    Callback cb_;
    std::tuple<Args...> args_;

    UncaughtExceptions hasUncaughtExceptions_;
};

template <typename Callback, typename... Args>
struct ScopeGuardOnExit : public ScopedGuardImpl<true, true, Callback, Args...> {};

template <typename Callback, typename... Args>
struct ScopeGuardOnSuccess : public ScopedGuardImpl<true, false, Callback, Args...> {};

template <typename Callback, typename... Args>
struct ScopeGuardOnFailure : public ScopedGuardImpl<false, true, Callback, Args...> {};

// Test transaction object

class TransactionObject {
public:
    virtual void undo() {};
    virtual void commit() {};
    virtual void finalize() {};
};

template <char Name = 'A'>
class Object : public TransactionObject {
public:
    Object() {
        std::cout << "-- " << Name << ": constructor" << std::endl;
    }
    Object(const Object&) {
        std::cout << "-- " << Name << ": copy constructor" << std::endl;
    }
    Object(Object&&) {
        std::cout << "-- " << Name << ": move constructor" << std::endl;
    }

    virtual ~Object() {
        std::cout << "-- " << Name << ": destructor" << std::endl;
    }

    void action() {
        std::cout << "-- " << Name << ": action" << std::endl;
    }

    virtual void undo() override {
        std::cout << "-- " << Name << ": undo" << std::endl;
    }

    virtual void commit() override {
        std::cout << "-- " << Name << ": commit" << std::endl;
    }

    virtual void finalize() {
        std::cout << "-- " << Name << ": finalize" << std::endl;
    }
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

// TEST SUITE

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)
#ifdef __COUNTER__
#define TEMPVAR(x) CONCAT(x, __COUNTER__)
#else
#define TEMPVAR(x) CONCAT(x, __LINE__)
#endif

struct ScopeGuardOnExitWrapper {};
template <typename Callback>
auto operator+(ScopeGuardOnExitWrapper, Callback&& cb) {
    return ScopeGuardOnExit<Callback>(std::forward<Callback>(cb));
}

struct ScopeGuardOnSuccessWrapper {};
template <typename Callback>
auto operator+(ScopeGuardOnSuccessWrapper, Callback&& cb) {
    return ScopeGuardOnSuccess<Callback>(std::forward<Callback>(cb));
}

struct ScopeGuardOnFailureWrapper {};
template <typename Callback>
auto operator+(ScopeGuardOnFailureWrapper, Callback&& cb) {
    return ScopeGuardOnFailure<Callback>(std::forward<Callback>(cb));
}

#define ON_SCOPE_EXIT \
auto TEMPVAR(SCOPE_STATE_EXIT) = ScopeGuardOnExitWrapper() + [&]()

#define ON_SCOPE_FAILURE \
auto TEMPVAR(SCOPE_STATE_FAILURE) = ScopeGuardOnFailureWrapper() + [&]()

#define ON_SCOPE_SUCCESS \
auto TEMPVAR(SCOPE_STATE_SUCCESS) = ScopeGuardOnSuccessWrapper() + [&]()

TEST(ScopeCommitFinalize, Test01) {
    Object<'A'> a;
    ON_SCOPE_EXIT { a.finalize(); };
    ON_SCOPE_FAILURE { a.undo(); };
    ON_SCOPE_SUCCESS { a.commit(); };

    a.action();
    std::cout << "--" << std::endl;

    // expected commit and then finalize
}

TEST(ScopeCommitFinalize, Test02) {
    try {
        Object<'A'> a;
        ON_SCOPE_EXIT { a.finalize(); };
        ON_SCOPE_FAILURE { a.undo(); };
        ON_SCOPE_SUCCESS { a.commit(); };

        a.action();
        std::cout << "--" << std::endl;

        throw std::invalid_argument( "action failed" );

        // expected undo and then finalize
    } catch (...) {}
}
