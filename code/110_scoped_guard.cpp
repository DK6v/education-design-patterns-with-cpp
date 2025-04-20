#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <typename D>

struct with_quantity_counter {
protected:
    static int quantity_;
    with_quantity_counter() { ++quantity_; }
    with_quantity_counter(const with_quantity_counter&) { ++quantity_; }

public:
    ~with_quantity_counter() { --quantity_; }

    static int quantity_counter() {
        return quantity_;
    }
};

template <typename D> int with_quantity_counter<D>::quantity_ = 0;

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
    void commit() const { commit_ = true; }

protected:
    ScopedGuardBase(const ScopedGuardBase& other) : commit_(other.commit_) {
        std::cout << "-- ScopedGuardBase: copy" << std::endl;
        other.commit();
    }
    ~ScopedGuardBase() {}

    // Set mutable, as it can be changed by commit() method.
    mutable bool commit_;

private:
    ScopedGuardBase& operator=(const ScopedGuardBase&) = delete;
};

template <typename Callback, typename... Args>
class ScopedGuardImpl : public ScopedGuardBase {
public:
    ScopedGuardImpl(const Callback & cb, Args... args)
        : cb_(cb), args_(std::make_tuple(std::forward<Args>(args)...)) {
//            std::cout << "-- ScopedGuard: constructor "
//                      << boost::typeindex::type_id_with_cvr<decltype(cb)>().pretty_name()
//                      << std::endl;
    }

    ~ScopedGuardImpl() {
        std::cout << "-- ScopedGuard: destructor" << std::endl;
        if (!commit_) { call_w_tuple_args(cb_, args_); }
    }

private:
    Callback cb_;
    std::tuple<Args...> args_;
};

/* If you want to create an instance of a class template

 * based on the arguments of the constructor, use the template function,
 * which can output the types of parameters and return values from the arguments.
 *

 * Not required from C++17
 */

template <typename Callback, typename... Args>
ScopedGuardImpl<Callback, Args...> MakeGuard(const Callback& cb, Args... args) {
    return ScopedGuardImpl<Callback, Args...>(cb, args...);
}

typedef const ScopedGuardBase& ScopedGuard;

// Test transaction object

class TransactionObject : public with_quantity_counter<TransactionObject> {
    public:
        virtual void undo() {};
        virtual void commit() {};
    };

template <char Type = 'A'>
class Object : public TransactionObject {
public:
    Object() {
        std::cout << "-- " << Type << ": constructor" << std::endl;
    }
    Object(const Object&) {
        std::cout << "-- " << Type << ": copy constructor" << std::endl;
    }
    Object(Object&&) {
        std::cout << "-- " << Type << ": move constructor" << std::endl;
    }

    virtual ~Object() {
        std::cout << "-- " << Type << ": destructor" << std::endl;
    }

    void action() {
        std::cout << "-- " << Type << ": action" << std::endl;
    }

    virtual void undo() override {
        std::cout << "-- " << Type << ": undo" << std::endl;
    }

    virtual void commit() override {
        std::cout << "-- " << Type << ": commit" << std::endl;
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

    // It is necessary to use a pointer to the 'a' object, otherwise it will be copied.
    auto cb = std::bind(&Object<'A'>::undo, &a);
    const ScopedGuardBase& g = MakeGuard(cb);
    g.commit();
}

TEST(ScopeCommitFinalize, Test02) {
    Object<'A'> a;
    a.action();

    // It is necessary to use a pointer to the 'a' object, otherwise it will be copied.
    ScopedGuardImpl g(std::bind(&Object<'A'>::undo, &a));
    // no commit, undo will be called
}
