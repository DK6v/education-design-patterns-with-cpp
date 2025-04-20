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

    Collection<T> sort() {
        auto sc = *this;
        std::sort(sc.begin(), sc.end());
        return sc;
    }

    Collection<T> filter(std::function<bool(const T&)> cond) {
        Collection<T> fc;
        for (auto & pair: c_) {
            if (cond(pair.second)) {
                fc.push(pair.second);
            }
        }
        return fc;
    }

    void print() {
        if (!c_.empty()) {
            auto it = c_.begin();
            std::cout << "{ " << (*it).first << ":" << (*it).second;
            while(++it != c_.end()) {
                std::cout << ", " << (*it).first << ":" << (*it).second;
            }
            std::cout << " }" << std::endl;
        }
        else {
            std::cout << "{}" << std::endl;
        }
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

    auto sc = c.sort();
    EXPECT_EQ(1, sc[0]);
    EXPECT_EQ(2, sc[1]);
    EXPECT_EQ(3, sc[2]);
    sc.print();

    auto cond = [](int t) -> bool { return (t <= 2) ? true : false; };
    auto fc = c.sort().filter(cond);
    EXPECT_EQ(1, fc[0]);
    EXPECT_EQ(2, fc[1]);
    EXPECT_EQ(2, fc.size());
    fc.print();
}