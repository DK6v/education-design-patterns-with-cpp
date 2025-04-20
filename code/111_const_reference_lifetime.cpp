#include "boost/type_index.hpp"
#include <gtest/gtest.h>

class Object {
public:
    Object() : id(total++) {
        std::cout << "-- Object[" << id << "]: constructor" << std::endl;
    }
    ~Object() {
        std::cout << "-- Object[" << id << "]: destructor" << std::endl;
    }
    Object(const Object& other) : id(total++) {
        std::cout << "-- Object[" << id << "]: copy constructor [" << other.id << "]" << std::endl;
    }
    Object(Object&& other) : id(total++) {
        std::cout << "-- Object[" << id << "]: move constructor [" << other.id << "]"<< std::endl;
    }

    void operator()() {
        std::cout << "-- Object[" << id << "]: operator()" << std::endl;
    }
    void bar() {
        std::cout << "-- Object[" << id << "]: bar()" << std::endl;
    }

    int id;

private:
    static inline int total = 1;
};

template <typename T>
class Callback {
public:
    Callback(T&& cb) : id(total++), cb_(std::forward<T>(cb)) {
        std::cout << "-- Callback[" << id << "]: constructor T&&" << std::endl;
    }
    Callback(const T& cb) : id(total++), cb_(cb) {
        std::cout << "-- Callback[" << id << "]: constructor const T&" << std::endl;
    }
    Callback(const Callback& other) : id(total++) {
        std::cout << "-- Callback[" << id << "]: copy constructor [" << other.id << "]" << std::endl;
    }
    Callback(Callback&& other) : id(total++) {
        std::cout << "-- Callback[" << id << "]: move constructor [" << other.id << "]"<< std::endl;
    }

    ~Callback() {
//        cb_();
        std::cout << "-- Callback[" << id << "]: destructor" << std::endl;
    }

    void bar() {
        std::cout << "-- Callback[" << id << "]: bar()" << std::endl;
    }

    int id;
    const T& cb_;

private:
    static inline int total = 1;
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

template <typename T>
void foo(const T& r) {
    const T& rl = r;
    (void)rl;
    std::cout << "-- foo" << std::endl;
}

TEST(CostReferenceLifetime, Test01) {
    Object a;
    std::cout << "-- 1" << std::endl;

    const Object& cr1 = Object();
    (void)cr1;
    std::cout << "-- 2" << std::endl;

    foo(std::bind(&Object::bar, a));
    std::cout << "-- 3" << std::endl;

    const Object& cr2 = cr1;
    (void)cr2;
    std::cout << "-- 4" << std::endl;

    auto cb = std::bind(&Object::bar, &a);
    std::cout << "-- 5" << std::endl;

    Callback cb1(std::bind(&Object::bar, a));
    // object 'a' will be copied and destroyed immediatelly !!!
    std::cout << "-- 6" << std::endl;

    Callback cb2(std::bind(&Object::bar, a));
    std::cout << "-- 7" << std::endl;

    Callback cb3(cb);
    std::cout << "-- 8" << std::endl;
}

#if false
const Object& getObject() {
    return Object(); // <- object will be immediatelly destroyed!!!
}

TEST(CostReferenceLifetime, Test02) {
    const auto & retval = getObject();
    // temporary object will be destroyed immediatelly!!!
    (void)retval;
    std::cout << "--" << std::endl;
}
#endif