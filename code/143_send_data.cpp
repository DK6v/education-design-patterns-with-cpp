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

    void send() { // non virtual, interface
        std::cout << "-- " << whoami() << ": send" << std::endl;
        send_data();
    };
protected:
    virtual void send_data() {
        std::cout << "-- " << Base::whoami() << ": send_data" << std::endl;
    }
};

class Derived : public Base {
public:
    std::string whoami() { return "Derived"; }
protected:
    virtual void send_data() override {
        std::cout << "-- " << Derived::whoami() << ": send_data" << std::endl;
        Base::send_data(); // protected, needs to call base method here

    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(TemplateMethod, Test01) {
    auto a = Base();
    std::cout << "-- whoami: " << a.whoami() << std::endl;
    a.send();
    std::cout << "--" << std::endl;
}

TEST(TemplateMethod, Test02) {
    auto da = Derived();
    std::cout << "-- whoami: " << da.whoami() << std::endl;
    da.send();
    std::cout << "--" << std::endl;

    auto pb = &da;
    std::cout << "-- whoami: " << pb->whoami() << std::endl;
    static_cast<Base*>(pb)->send();
    std::cout << "--" << std::endl;
}
