#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <typename T>
class has_unique_id {
public:
    has_unique_id() : id_(++total_) {}
    virtual int id() const { return id_; }
private:
    mutable int id_; // cannot be const, otherwise cannot generate default constructors
    static inline int total_ = 0;
};

template <typename T>
struct printable : public has_unique_id<T> {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        out << boost::typeindex::type_id_with_cvr<decltype(rhs)>().pretty_name()
            << "[" << rhs.id() << "]";
        return out;

    }
};

class Base : public printable<Base> {
public:
    Base() = default;
    virtual ~Base() = default;
    virtual std::string whoami() { return "Base"; }

    virtual void preamble() {};
    virtual void postscript() {};

    void action() { // non virtual, interface
        preamble();
        std::cout << "-- " << whoami() << std::endl;
        postscript();
    };
};

class Derived_A : public Base {
public:
    std::string whoami() { return "Derived_A"; }
    virtual void preamble() override {
        std::cout << "-- " << whoami() << ": preable" << std::endl;
    }
};

class Derived_B : public Base {
public:
    std::string whoami() { return "Derived_B"; }
    virtual void postscript() override {
        std::cout << "-- " << whoami() << ": postscript" << std::endl;
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(TemplateMethod, Test01) {
    auto a = Base();
    std::cout << "-- whoami: " << a.whoami() << std::endl;
    a.action();
    std::cout << "--" << std::endl;
}

TEST(TemplateMethod, Test02) {
    auto da = Derived_A();
    std::cout << "-- whoami: " << da.whoami() << std::endl;
    da.action();
    std::cout << "--" << std::endl;

    auto pb = &da;
    std::cout << "-- whoami: " << pb->whoami() << std::endl;
    static_cast<Base*>(pb)->action();
    std::cout << "--" << std::endl;
}

TEST(TemplateMethod, Test03) {
    auto db = Derived_B();
    std::cout << "-- whoami: " << db.whoami() << std::endl;
    db.action();
    std::cout << "--" << std::endl;

    auto pb = &db;
    std::cout << "-- whoami: " << pb->whoami() << std::endl;
    static_cast<Base*>(pb)->action();
    std::cout << "--" << std::endl;
}
