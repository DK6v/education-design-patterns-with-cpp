#include <gtest/gtest.h>

#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template <char Name = 'A'>
class Object {
public:
    Object() : id_(total_++) {
        std::cout << "-- " << *this << ": constructor" << std::endl;
    }
    ~Object() {
        std::cout << "-- " << *this << ": destructor" << std::endl;
    }
    Object(const Object& other) : id_(total_++) {
        std::cout << "-- " << *this << ": copy constructor [" << other.id << "]" << std::endl;
    }
    Object(Object&& other) : id_(total_++) {
        std::cout <<"-- " << *this << ": move constructor [" << other.id << "]"<< std::endl;
    }

    friend std::ostream& operator<<(std::ostream& out, const Object& rhs) {
        out << "Object<" << Name << ">[" << rhs.id_ << "]";
        return out;

    }

private:
    int id_;
    static inline int total_ = 1;
};

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(ObjectCounter, AcquireRelease) {

    Object<'A'> a;
    std::cout << "-- " << a << ": Hello!!!" << std::endl;
}
