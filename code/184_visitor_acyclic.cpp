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

class Object;
class Number;
class String;

class Visitor {
public:
    virtual ~Visitor() = default;
};

class INumberVisitor {
public:
    virtual void visit(Number& item) = 0;
};

class IStringVisitor {
public:
    virtual void visit(String& item) = 0;
};

class IObjectVisitor {
public:
    virtual void visit(Object& item) = 0;
};

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

class Number : public Item,
               public printable<Number> {
public:
    Number(const std::string& name, long value): Item(name), value_(value) {};
    Number(std::string&& name, long value): Item(std::move(name)), value_(value) {};

    long getValue() const { return value_; }
    void setValue(long value) { value_ = value; }

    virtual void accept(Visitor& v) override {

        try {
            dynamic_cast<INumberVisitor&>(v).visit(*this);
        }
        catch (const std::bad_cast&) {
            std::cout << "Number: skip visitor" << std::endl;

        }
    }

private:
    long value_;
};

class String : public Item,
               public printable<String> {
public:
    template <typename N = std::string,
              typename V = std::string,
              typename = std::enable_if_t<
                  std::is_convertible_v<N, std::string> &&
                  std::is_convertible_v<V, std::string>>>
    String(N &&name, V &&value) : Item(std::forward<N>(name)), value_(std::forward<V>(value)) {}

    const std::string& getValue() const { return value_; }
    void setValue(const std::string& value) { value_ = value; }
    void setValue(std::string&& value) { value_ = std::move(value); }

    virtual void accept(Visitor& v) override {

        try {
            dynamic_cast<IStringVisitor&>(v).visit(*this);
        }
        catch (const std::bad_cast&) {
            std::cout << "String: skip visitor" << std::endl;

        }
    }

private:
    std::string value_;
};

class Object : public Item,
               public printable<Object> {
public:
    Object() : Item("object") {};
    virtual void accept(Visitor& v) override {

        try {
            dynamic_cast<IObjectVisitor&>(v).visit(*this);
        }
        catch (const std::bad_cast&) {
            std::cout << "Object: skip visitor" << std::endl;

        }
    }

    void add(std::unique_ptr<Item> ptr) {
        items_.push_back(std::move(ptr));
    }

    auto begin() { return items_.begin(); }
    auto end() { return items_.end(); }

private:
    std::vector<std::unique_ptr<Item>> items_;
};

class TestVisitor : public Visitor,
                    public INumberVisitor,
                    public IStringVisitor,
                    public IObjectVisitor
{
public:
    virtual void visit(Object &item) override { std::cout << item.getName() << " visited" << std::endl; }
    virtual void visit(Number &item) override { std::cout << item.getName() << " visited" << std::endl; }
    virtual void visit(String &item) override { std::cout << item.getName() << " visited" << std::endl; }
};

class Serivalizer : public Visitor,
                    public INumberVisitor,
                    public IStringVisitor,
                    public IObjectVisitor
{
public:
    virtual void visit(Object& item) override {
        ident();
        ss_ << "{" << std::endl;

        ++shift_;
        auto it = item.begin();
        while (it != item.end()) {
            ident();
            (*it++)->accept(*this);
            if (it != item.end()) {
                ss_ << ",";
            }
            ss_ << std::endl;
        }
        --shift_;

        ident();
        ss_ << "}";
    }

    virtual void visit(Number& item) override {
        if (!shift_) { ss_ << "{" << std::endl << "  "; }
        ss_ << "\"" << item.getName() << "\": " << item.getValue();
        if (!shift_) { ss_ << std::endl << "}"; }
    }
    virtual void visit(String& item) override {
        if (!shift_) { ss_ << "{" << std::endl << "  "; }
        ss_ << "\"" << item.getName() << "\": \"" << item.getValue() << "\"";
        if (!shift_) { ss_ << std::endl << "}"; }
    }

    std::string result() { return ss_.str(); }
private:
    void ident() {
        for (int ix = shift_; ix > 0; --ix) {
            ss_ << "  ";
        }
    }
private:
    std::stringstream ss_;
    int shift_ = 0;
};

class SumVisitor : public Visitor,
                   public INumberVisitor,
                   public IObjectVisitor
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

TEST(Visitor, Test03) {
    {
        std::unique_ptr<Object> root(new Object());
        root->add(std::make_unique<Number>(Number("a", 100)));
        root->add(std::make_unique<String>(String("b", "text")));

        Serivalizer serializer;
        root->accept(serializer);

        std::cout << serializer.result() << std::endl;

    }
    std::cout << "-- DONE" << std::endl;
}

TEST(Visitor, Test04) {
    {
        std::unique_ptr<Number> root(new Number("a", 100));

        Serivalizer serializer;
        root->accept(serializer);

        std::cout << serializer.result() << std::endl;

    }
    std::cout << "-- DONE" << std::endl;
}

TEST(Visitor, Test05) {
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
