#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <typename T>
class Value {
public:
    using basic_type = T;
    using value_type = Value;

    explicit Value() : val_(T()) {}
    explicit Value(T val) : val_(val) {}

    Value(const Value& other) : val_(other.val_) {}
    value_type& operator=(const basic_type& other) { val_ = val_; return *this; }
    value_type& operator=(const value_type& other) { val_ = other.val_; return *this; }

    friend std::ostream& operator<<(std::ostream& out, const value_type& rhs) {
        out << rhs.val_;
        return out;

    }

protected:
    T val_;
};

template <typename V>
class HasEqual : public V {
public:
    using V::V;
    using value_type = V;
    using basic_type = V::basic_type;

    HasEqual(const value_type& value) : value_type(value) {}

    friend bool operator==(const HasEqual& lhs, const HasEqual& rhs) {
        return lhs.val_ == rhs.val_;
    }
    friend bool operator==(const HasEqual& lhs, const basic_type& value) {
        return lhs.val_ == value;
    }
    friend bool operator==(const basic_type& value, const HasEqual& rhs) {
        return value == rhs.val_;
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Adapter, Test01) {
    {
        Value<std::string> val("A");
        std::cout << "Value: " << val << std::endl;
    }
    std::cout << "-- end of context" << std::endl;
}

TEST(Adapter, Test02) {
    {
        Value<std::string> val("B");
        std::cout << "Value: " << val << std::endl;

        auto hasEqual = HasEqual(val);
        std::cout << "Value: " << hasEqual << std::endl;

        EXPECT_EQ(hasEqual, std::string("B"));
        EXPECT_EQ(hasEqual, "B");

        EXPECT_EQ(std::string("B"), hasEqual);
        EXPECT_EQ("B", hasEqual);
    }
    std::cout << "-- end of context" << std::endl;
}
