#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include <list>

struct Animal {
public:
    enum Type { CAT, DOG};
    Animal(Type type) : type(type) {}
    const Type type;
};

template <typename D>
class BaseVisitor {
public:
    template <typename iterator>
    void visit(iterator from, iterator to) {
        for (iterator it = from; it != to; ++it) {
            this->visit(*it);
        }
    }

private:
    BaseVisitor() {}

    /*

     * This means that the friend's member declarations and definitions
     * can access private and protected members of this class and
     * also that the friend can inherit from private and protected members of this class.
     */
    friend D;

    D& derived() {
        return *static_cast<D*>(this);
    }

    void visit(const Animal& animal) {
        switch(animal.type) {
            case Animal::CAT:
                derived().visit_cat(animal);
                break;
            case Animal::DOG:
                derived().visit_dog(animal);
                break;
            default:
                break;
        }
    }

    void visit_cat(const Animal& animal) {
        (void)animal;
        std::cout << "BaseVisitor::CAT" << std::endl;
    }

    void visit_dog(const Animal& animal) {
        (void)animal;
        std::cout << "BaseVisitor::DOG" << std::endl;
    }
};

class MyVisitor : public BaseVisitor<MyVisitor> {
    friend class BaseVisitor<MyVisitor>;
    void visit_cat(const Animal& animal) {
        (void)animal;
        std::cout << "MyVisitor::CAT" << std::endl;
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(CRTP_Visitor, Test1) {
    std::list<Animal> animals;

    animals.push_back(Animal(Animal::DOG));
    animals.push_back(Animal(Animal::CAT));

    MyVisitor().visit(animals.cbegin(), animals.cend());
}