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

    void operator()() const {
        std::cout << "-- Object[" << id << "]: operator()" << std::endl;
    }
    void bar() {
        std::cout << "-- Object[" << id << "]: bar()" << std::endl;
    }

    int id;

private:
    static inline int total = 1;
};

template <typename T, char N = 'A'>
class Callback {
public:

    Callback(T&& cb) : id(total++), cb_(std::forward<T>(cb)) {
        std::cout << "-- Callback<" << N << ">[" << id << "]: constructor T&&" << std::endl;
    }
    Callback(const T& cb) : id(total++), cb_(cb) {
        std::cout << "-- Callback<" << N << ">[" << id << "]: constructor const T&" << std::endl;
    }
    Callback(const Callback<T, N>& other) : id(total++) {
        std::cout << "-- Callback<" << N << ">[" << id << "]: copy constructor [" << other.id << "]" << std::endl;
    }
    Callback(Callback<T, N>&& other) : id(total++) {
        std::cout << "-- Callback<" << N << ">[" << id << "]: move constructor [" << other.id << "]"<< std::endl;
    }

    ~Callback() {
        cb_();
        std::cout << "-- Callback<" << N << ">[" << id << "]: destructor" << std::endl;
    }

    void bar() {
        std::cout << "-- Callback<" << N << ">[" << id << "]: bar()" << std::endl;
    }

    void operator()() const {
        std::cout << "-- Callback<" << N << ">[" << id << "]: operator()" << std::endl;
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
    std::cout << "--" << std::endl;
}

TEST(CostReferenceLifetime, Test02) {
    Object a;
    Callback<Object> c(a);
    std::cout << "--" << std::endl;
}

TEST(CostReferenceLifetime, Test03) {
    Callback<Object> c{Object()};
    std::cout << "--" << std::endl;
}

TEST(CostReferenceLifetime, Test04) {
    Callback<Callback<Object, 'B'>, 'A'> c(Callback<Object, 'B'>{Object()});
    std::cout << "--" << std::endl;
}