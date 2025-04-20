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

class VisitorBase {
public:
    virtual ~VisitorBase() = default;
};

template <typename Visitable>
class Visitor {
public:
    virtual void visit(Visitable& item) = 0;
};

template <typename Base, typename Visitable>
class VisitableBase : public Base {
public:
    using Base::Base;
    void accept(VisitorBase& v) override {
        try {
            dynamic_cast<Visitor<Visitable>&>(v).visit(dynamic_cast<Visitable&>(*this));
        }
        catch (const std::bad_cast&) {
            std::cout <<

                boost::typeindex::type_id<decltype(*this)>().pretty_name() <<
                " : skip visitor" << std::endl;

        }
    }
};

class Item {
public:
    Item(const std::string& name) : name_(name) {}
    Item(std::string&& name) : name_(std::move(name)) {}
    virtual ~Item() = default;

    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    void setName(std::string&& name) { name_ = std::move(name); }

    virtual void accept(VisitorBase& v) = 0;

private:
    std::string name_;
};

class Number : public VisitableBase<Item, Number>,
               public printable<Number> {
public:
    using VisitableBase<Item, Number>::VisitableBase;

    Number(const std::string& name, long value): VisitableBase(name), value_(value) {};
    Number(std::string&& name, long value): VisitableBase(std::move(name)), value_(value) {};

    long getValue() const { return value_; }
    void setValue(long value) { value_ = value; }

private:
    long value_;
};

class String : public VisitableBase<Item, String>,
               public printable<String> {
public:
    using VisitableBase<Item, String>::VisitableBase;

    template <typename N = std::string,
              typename V = std::string,
              typename = std::enable_if_t<
                  std::is_convertible_v<N, std::string> &&
                  std::is_convertible_v<V, std::string>>>
    String(N &&name, V &&value) : VisitableBase(std::forward<N>(name)), value_(std::forward<V>(value)) {}

    const std::string& getValue() const { return value_; }
    void setValue(const std::string& value) { value_ = value; }
    void setValue(std::string&& value) { value_ = std::move(value); }

private:
    std::string value_;
};

class Object : public VisitableBase<Item, Object>,
               public printable<Object> {
public:
    using VisitableBase<Item, Object>::VisitableBase;

    Object() : VisitableBase("object") {};

    void add(std::unique_ptr<Item> ptr) {
        items_.push_back(std::move(ptr));
    }

    auto begin() { return items_.begin(); }
    auto end() { return items_.end(); }

private:
    std::vector<std::unique_ptr<Item>> items_;
};

template <typename ... V> struct Visitors;

template <typename V1>
struct Visitors<V1> : public VisitorBase, public Visitor<V1> {};

template <typename V1, typename ... V>
struct Visitors<V1, V...> : public Visitor<V1>, public Visitors<V...> {};

class TestVisitor : public Visitors<Number, String, Object>
{
public:
    void visit(Number &item) override { std::cout << item.getName() << " visited" << std::endl; }
    void visit(String &item) override { std::cout << item.getName() << " visited" << std::endl; }
    void visit(Object &item) override { std::cout << item.getName() << " visited" << std::endl; }
};

class SumVisitor : public VisitorBase,
                   public Visitor<Number>,
                   public Visitor<Object>
{
public:
    virtual void visit(Number &item) override { sum_ += item.getValue(); }
    virtual void visit(Object &item) override {
        auto it = item.begin();
        while (it != item.end()) {
            (*it++)->accept(*this);
        }
    }

    long sum() { return sum_; }
private:
    long sum_ = 0;
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(VisitorTest, Test01) {
    {
        std::unique_ptr<Object> root(new Object());
        root->add(std::make_unique<Number>(Number("a", 100)));
        root->add(std::make_unique<String>(String("b", "text")));

        TestVisitor visitor;
        root->accept(visitor);
    }
    std::cout << "-- DONE" << std::endl;
}

TEST(Visitor, Test02) {
    {
        std::unique_ptr<Object> root(new Object());
        root->add(std::make_unique<Number>(Number("a", 100)));
        root->add(std::make_unique<String>(String("b", "text")));

        SumVisitor visitor;
        root->accept(visitor);

        std::cout << "SUM: " << visitor.sum() << std::endl;

    }
    std::cout << "-- DONE" << std::endl;
}
