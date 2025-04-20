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

using Visitor = VisitorTypes<class Number, class String, class Object>;

class Item {
public:
    Item(const std::string& name) : name_(name) {}
    Item(std::string&& name) : name_(std::move(name)) {}
    virtual ~Item() = default;

    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    void setName(std::string&& name) { name_ = std::move(name); }

    virtual void accept(Visitor& v) = 0;

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

class Number : public Visitable<Number>,
               public printable<Number>
{
public:
    Number(const std::string &name, long value) : Visitable(name), value_(value) {};
    Number(std::string &&name, long value) : Visitable(std::move(name)), value_(value) {};

    long getValue() const { return value_; }
    void setValue(long value) { value_ = value; }

private:
    long value_;
};

class String : public Visitable<String>,
               public printable<String>
{
public:
    template <typename N = std::string,
              typename V = std::string,
              typename = std::enable_if_t<
                  std::is_convertible_v<N, std::string> &&
                  std::is_convertible_v<V, std::string>>>
    String(N &&name, V &&value) : Visitable(std::forward<N>(name)), value_(std::forward<V>(value)) {}

    const std::string &getValue() const { return value_; }
    void setValue(const std::string &value) { value_ = value; }
    void setValue(std::string &&value) { value_ = std::move(value); }

private:
    std::string value_;
};

class Object : public Visitable<Object>,
               public printable<Object> {
public:
    Object() : Visitable("object") {};
    virtual void accept(Visitor& v) override { v.visit(*this); }

    void add(std::unique_ptr<Item> ptr) {
        items_.push_back(std::move(ptr));
    }

    auto begin() { return items_.begin(); }
    auto end() { return items_.end(); }

private:
    std::vector<std::unique_ptr<Item>> items_;
};

class TestVisitor : public Visitor {
public:
    virtual void visit(Object& item) override { std::cout << item.getName() << " visited" << std::endl; }
    virtual void visit(Number& item) override { std::cout << item.getName() << " visited" << std::endl; }
    virtual void visit(String& item) override { std::cout << item.getName() << " visited" << std::endl; }
};

template <typename Base, typename ...>
class LambdaVisitor;

template <typename Base, typename T1, typename F1>
class LambdaVisitor<Base, VisitorTypes<T1>, F1> :
    private F1,
    public Base {
public:
    LambdaVisitor(F1&& f) : F1(std::move(f)) {}
    LambdaVisitor(const F1& f) : F1(f) {}

    void visit(T1& item) override { F1::operator()(item); };
};

template <typename Base,
          typename T1, typename... T,
          typename F1, typename... F>
class LambdaVisitor<Base, VisitorTypes<T1, T...>, F1, F...> :
    private F1,
    public LambdaVisitor<Base, VisitorTypes<T...>, F...>
{
public:
    using LambdaVisitor<Base, VisitorTypes<T...>, F...>::visit;

    LambdaVisitor(F1&& head, F&& ... tail) :
        F1(std::move(head)),
        LambdaVisitor<Base, VisitorTypes<T...>, F...>(std::forward<F>(tail)...) {}
    LambdaVisitor(const F1& head, F&& ... tail) :
        F1(head),
        LambdaVisitor<Base, VisitorTypes<T...>, F...>(std::forward<F>(tail)...) {}

    void visit(T1& item) override { F1::operator()(item); };
};

template <typename Base, typename ... F>
auto make_visitor(F&& ... f) {
    return LambdaVisitor<Base, Base, F ...>(std::forward<F>(f)...);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Visitor, Test02) {
    {
        std::unique_ptr<Object> root(new Object());
        root->add(std::make_unique<Number>(Number("a", 100)));
        root->add(std::make_unique<String>(String("b", "text")));

        TestVisitor visitor;
        root->accept(visitor);
    }
    std::cout << "-- DONE" << std::endl;
}

TEST(Visitor, Test3) {
    {
        std::unique_ptr<String> root(new String("a", "text"));

        auto visitor = make_visitor<Visitor>(
            [] (Number& item) { std::cout << item.getName() << " visited" << std::endl; },
            [] (String& item) { std::cout << item.getName() << " visited" << std::endl; },
            [] (Object& item) { std::cout << item.getName() << " visited" << std::endl; }
        );

        root->accept(visitor);
    }
    std::cout << "-- DONE" << std::endl;
}
