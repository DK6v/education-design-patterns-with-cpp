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

// Scope guard object, callback if there are no commit

class ScopedGuardBase {
public:
    ScopedGuardBase() : commit_(false) {}

    // Set the commit method "const" to call it from the copy constructor.
    void commit() const noexcept { commit_ = true; }

protected:
    ScopedGuardBase(const ScopedGuardBase&& other) : commit_(other.commit_) {
        std::cout << "-- ScopedGuardBase: move" << std::endl;
        other.commit();
    }
    ~ScopedGuardBase() {}

    // Set mutable, as it can be changed by commit() method.
    mutable bool commit_;

private:
    ScopedGuardBase& operator=(const ScopedGuardBase&) = delete;
};

template <typename Callback, typename... Args>
class ScopedGuard : public ScopedGuardBase {
public:
    template <typename = void>
    ScopedGuard(const Callback& cb, const Args&... args)
        : cb_(cb), args_(args...) {
        std::cout << "-- ScopedGuard: constructor (const Callback&, const Args&...) -> "
            << boost::typeindex::type_id_with_cvr<decltype(cb)>().pretty_name()
            << std::endl;
    }

    ~ScopedGuard() {
        std::cout << "-- ScopedGuard: destructor" << std::endl;
        if (!commit_) { call_w_tuple_args(cb_, args_); }
    }

    ScopedGuard(ScopedGuard&& other)
        : ScopedGuardBase(std::move(other)),
          cb_(std::move(other.cb_)),
          args_(std::move(other.args_)) {

        std::cout << "-- ScopedGuardImpl: move" << std::endl;
    }

private:
    Callback cb_;
    std::tuple<Args...> args_;
};

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

void foo() {
    std::cout << "-- foo: commit" << std::endl;
}

TEST(ScopeCommitFinalize, Test01) {
    Object<'A'> a;
    a.action();

    ScopedGuard g([&] { a.undo(); });
    g.commit();

    std::cout << "--" << std::endl;
    // Undo will not be executed.
}

TEST(ScopeCommitFinalize, Test02) {
    Object<'A'> a;
    a.action();

    ScopedGuard g([&] { a.undo(); });

    std::cout << "--" << std::endl;
    // No commit, undo will be performed.
}

TEST(ScopeCommitFinalize, Test03) {
    Object<'A'> a;
    a.action();

    auto cb = [&](){ a.undo(); };
    ScopedGuard g(cb);

    std::cout << "--" << std::endl;
    // No commit, undo will be performed.
}

TEST(ScopeCommitFinalize, Test04) {
    Object<'A'> a;
    a.action();

    ScopedGuard ga([&](){ a.undo(); });
    ScopedGuard fa([&](){ a.finalize(); });

    Object<'B'> b;
    b.action();

    ga.commit();

    std::cout << "--" << std::endl;
    // Always run the A::finalize() here.
}
