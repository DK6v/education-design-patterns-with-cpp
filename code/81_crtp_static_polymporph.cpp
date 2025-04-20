#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

template <typename D>
class Base {
public:
    Base() : c_(0) {}
    ~Base() {}
    D* derived() { return static_cast<D*>(this); }

    void f() { derived()->f_impl(); }
    char get() const { return c_; }

protected:
    char c_;
};

template <typename D>
void destroy(Base<D>* p) { delete static_cast<D*>(p); }

class A : public Base<A> {
    void f_impl() { c_ = 'A'; }
    friend class Base<A>;
};

class B : public Base<B> {
    void f_impl() { c_ = 'B'; }
    friend class Base<B>;
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(CRTP_StaticPolymorphism, Test1) {
    Base<A>* pa = new A();
    pa->f();
    EXPECT_EQ('A', pa->get());
    destroy(pa);

    Base<B>* pb = new B();
    pb->f();
    EXPECT_EQ('B', pb->get());

    void* p = pa;
    // cannot convert ‘Base<B>*’ to ‘Base<A>*’ in assignment
    // p = pb;

    destroy(pa);
    destroy(pb);
}