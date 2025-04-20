#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include <map>
#include <utility>

// template <typename T> class Collection;

template<template<class> class C, class T>
class has_index_iterator {
public:
    struct iterator {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = size_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

    public:
        iterator(C<T>& collection, size_t from = 0) : c_(collection), ix_(from) {}
        iterator(const iterator& rhs) : c_(rhs.c_), ix_(rhs.ix_) {}

        iterator& operator=(const iterator& rhs) { c_ = rhs.c_; ix_ = rhs.ix_; return *this; }

        reference operator[](difference_type ix) const { return c_[ix]; }

        reference operator*() const { return c_[ix_]; }
        pointer operator->() const { return &(c_[ix_]); }

        // Prefix, modify and return this
        iterator& operator++() { ++ix_; return *this; }
        iterator& operator--() { --ix_; return *this; }
        // Posfix, make copy then modyfy

        iterator operator++(int) { iterator retval = *this; ++(*this); return retval; }
        iterator operator--(int) { iterator retval = *this; --(*this); return retval; }

        iterator& operator+=(difference_type n) { ix_ += n; return *this; }
        iterator& operator-=(difference_type n) { ix_ -= n; return *this; }
        iterator& operator*=(difference_type n) { ix_ *= n; return *this; }
        iterator& operator/=(difference_type n) { ix_ *= n; return *this; }
        iterator operator+(difference_type n) { auto retval = *this; retval += n; return retval; }
        iterator operator-(difference_type n) { auto retval = *this; retval -= n; return retval; }
        iterator operator*(difference_type n) { auto retval = *this; retval *= n; return retval; }
        iterator operator/(difference_type n) { auto retval = *this; retval /= n; return retval; }

        difference_type operator-(iterator other) { return ix_ - other.ix_; }

        // Make comparable with another iterator, define free functions
        friend bool operator==(const iterator& lhs, const iterator& rhs) { return lhs.ix_ == rhs.ix_; }
        friend bool operator!=(const iterator& lhs, const iterator& rhs) { return lhs.ix_ != rhs.ix_; }
        friend bool operator>(const iterator& lhs, const iterator& rhs) { return lhs.ix_ > rhs.ix_; }
        friend bool operator<(const iterator& lhs, const iterator& rhs) { return lhs.ix_ < rhs.ix_; }
        friend bool operator>=(const iterator& lhs, const iterator& rhs) { return lhs.ix_ >= rhs.ix_; }
        friend bool operator<=(const iterator& lhs, const iterator& rhs) { return lhs.ix_ <= rhs.ix_; }

    private:
        C<T>& c_;
        size_t ix_;
    };

    // Prepare container
    iterator begin() { return iterator(derived()); }
    iterator end() { return iterator(derived(), derived().size()); }

protected:
    ~has_index_iterator() {}
    auto & derived() { return *(static_cast<C<T>*>(this)); }
};

template <typename T>
class Collection : public has_index_iterator<Collection, T> {

public:
    Collection() {}
    Collection(std::initializer_list<T> list) {
        for (auto & t : list) {
            push(t);
        }
    }

    int size() { return c_.size(); }
    T& operator[](int ix) { return c_[ix]; }

    void push(const T& t) { c_.insert({c_.size(), t}); }
    T& pop() {

        T t = c_[size() - 1];
        c_.erase(size() - 1);
        return t;
    }

    void f() {
        auto it = c_.begin();
    }

private:
    std::map<int, T> c_;
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(CRTP_Iterator, Test1) {
    Collection<int> c = {3, 1, 2};
    EXPECT_EQ(3, c.size());
    EXPECT_EQ(3, c[0]);
    EXPECT_EQ(1, c[1]);
    EXPECT_EQ(2, c[2]);

    using std::swap;
    swap(c[0], c[2]);
    EXPECT_EQ(2, c[0]);
    EXPECT_EQ(1, c[1]);
    EXPECT_EQ(3, c[2]);

    auto it = c.begin();
    std::cout << *it << std::endl;

    std::sort(c.begin(), c.end());
    for (auto & t: c) {
        std::cout << t << " ";
    }
    std::cout << std::endl;
}