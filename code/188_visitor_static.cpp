#include "boost/type_index.hpp"
#include <gtest/gtest.h>

#include <string>

template <typename T>
class has_unique_id {
public:
    has_unique_id() : id_(++total_) {}
    int uid() const { return id_; }
private:
    mutable int id_; // cannot be const, otherwise cannot generate default constructors
    static inline int total_ = 0;
};

template <typename T>
struct printable : private has_unique_id<T> {
    friend std::ostream& operator<<(std::ostream& out, const T& rhs) {
        return out << boost::typeindex::type_id<decltype(rhs)>().pretty_name()
                   << "[" << rhs.uid() << "]";
    }
};

template <typename ... Types>
class VisitorTypes;

template <typename T>
class VisitorTypes<T> {
public:
    virtual void visit(T&) = 0;
};

template <typename T, typename ... Types>
class VisitorTypes<T, Types ...> : public VisitorTypes<Types ...> {
public:
    using VisitorTypes<Types ...>::visit;
    virtual void visit(T&) = 0;
};

using Visitor = VisitorTypes<class Number, class String>;

class Item {
public:
    Item(const std::string& name) : name_(name) {}
    Item(std::string&& name) : name_(std::move(name)) {}
    virtual ~Item() = default;

    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    void setName(std::string&& name) { name_ = std::move(name); }

    template <typename This, typename Visitor>
    static void accept(This& t, Visitor& v) {
        v.visit(t);
    }

private:
    std::string name_;
};

template <typename Derived>
class Visitable : public Item {
public:
    using Item::Item;
    void accept(Visitor& v) override {
        v.visit(static_cast<Derived&>(*this));
    };
};

class Number : public Item,
               public printable<Number>
{
public:
    using Item::Item;

    Number(const std::string &name, long value) : Item(name), value_(value) {};
    Number(std::string &&name, long value) : Item(std::move(name)), value_(value) {};

    long getValue() const { return value_; }
    void setValue(long value) { value_ = value; }

private:
    long value_;
};

class String : public Item,
               public printable<String>
{
public:
    using Item::Item;

    template <typename N = std::string,
              typename V = std::string,
              typename = std::enable_if_t<
                  std::is_convertible_v<N, std::string> &&
                  std::is_convertible_v<V, std::string>>>
    String(N &&name, V &&value) : Item(std::forward<N>(name)), value_(std::forward<V>(value)) {}

    const std::string &getValue() const { return value_; }
    void setValue(const std::string &value) { value_ = value; }
    void setValue(std::string &&value) { value_ = std::move(value); }

private:
    std::string value_;
};

class TestVisitor : public Visitor {
public:
    virtual void visit(Number& item) override { std::cout << item.getName() << " visited" << std::endl; }
    virtual void visit(String& item) override { std::cout << item.getName() << " visited" << std::endl; }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Visitor, Test01) {
    {
        std::unique_ptr<Number> root(new Number("a", 100));

        TestVisitor visitor;
        Item::accept(*root, visitor);
    }
    std::cout << "-- DONE" << std::endl;
}

TEST(Visitor, Test02) {
    {
        std::unique_ptr<String> root(new String("b", "text"));

        TestVisitor visitor;
        Item::accept(*root, visitor);
    }
    std::cout << "-- DONE" << std::endl;
}