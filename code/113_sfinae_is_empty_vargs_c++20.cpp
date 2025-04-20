#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <typename T, typename... Args>
class Object {
public:
    Object() {
        std::cout << "-- Object: constructor" << std::endl;
    }

    Object(Object&& other) : t_(std::move(other.t_)) {
        std::cout << "-- Object: move constructor" << std::endl;
    }

    Object(const Object& other) : t_(other.t_) {
        std::cout << "-- Object: copy constructor" << std::endl;
    }

    ~Object() {
        std::cout << "-- Object: destructor" << std::endl;
    }

    template <typename = void> requires (sizeof...(Args) == 0)
    explicit Object(const T& t)
    : t_(t) {
        std::cout << "-- Object: constructor(const T&)" << std::endl;
    }

    template <typename = void> requires (sizeof...(Args) != 0)
    explicit Object(const T& t, const Args&... args)
    : t_(t), args_(args...) {
        std::cout << "-- Object: constructor(const T&, const Args&...)" << std::endl;
    }

    T t_;
    std::tuple<Args...> args_;
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(SfinaeIsEmptyArgs, Test01) {
    int i = 1;
    Object a(i);
}

TEST(SfinaeIsEmptyArgs, Test02) {
    Object a(1);
}

TEST(SfinaeIsEmptyArgs, Test03) {
    Object a(1, 2);
}
