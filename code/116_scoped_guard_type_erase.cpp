#include "boost/type_index.hpp"
#include <gtest/gtest.h>

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

class ScopedGuard : public ScopedGuardBase {
public:
    template <typename Callback>
    explicit ScopedGuard(Callback&& cb) : cb_(std::forward<Callback>(cb)) {
        std::cout << "-- ScopedGuard: constructor (Callback&&) -> "
            << boost::typeindex::type_id_with_cvr<decltype(cb)>().pretty_name()
            << std::endl;
    }

    template <typename Callback>
    explicit ScopedGuard(const Callback& cb) : cb_(cb) {
        std::cout << "-- ScopedGuard: constructor (const Callback&) -> "
            << boost::typeindex::type_id_with_cvr<decltype(cb)>().pretty_name()
            << std::endl;
    }

    ~ScopedGuard() {
        std::cout << "-- ScopedGuard: destructor" << std::endl;
        if (!commit_) { cb_(); }
    }

    ScopedGuard(ScopedGuard&& other)
        : ScopedGuardBase(std::move(other)),
          cb_(std::move(other.cb_)) {

        std::cout << "-- ScopedGuardImpl: move" << std::endl;
    }

private:
    std::function<void()> cb_;
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

    const auto & cb = [&](){ a.undo(); };
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
