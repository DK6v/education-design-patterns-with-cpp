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

class Visitor;

class Geometry {
public:
    virtual ~Geometry() = default;
    virtual void accept(Visitor& v) = 0;

    enum Type { ANY = 100, POINT, LINE };
    virtual Geometry::Type type() const = 0;
};

class Point;
class Line;

class Visitor {
public:
    virtual void visit(long&) = 0;
    virtual void visit(Point&) = 0;
    virtual void visit(Line&) = 0;

    virtual void visit(Geometry::Type&) = 0;
    static Geometry* make_geometry(Geometry::Type type);
};

class Point : public Geometry,
              public printable<Point> {
public:
    Point() = default;
    Point(long x, long y): x_(x), y_(y) {};

    virtual void accept(Visitor& v) override {

        v.visit(x_);
        v.visit(y_);
    }

    virtual Geometry::Type type() const { return Geometry::Type::POINT; };

    friend bool operator==(const Point& lhs, const Point& rhs) {
        return (lhs.x_ == rhs.x_) && (lhs.y_ == rhs.y_);
    }

private:
    long x_, y_;
};

class Line : public Geometry,
             public printable<Line> {
public:
    Line() = default;

    Line(Point a, Point b): a_(a), b_(b) {};
    virtual void accept(Visitor& v) override {

        v.visit(a_);
        v.visit(b_);
    }

    virtual Geometry::Type type() const { return Geometry::Type::LINE; };

    friend bool operator==(const Line& lhs, const Line& rhs) {
        return (lhs.a_ == rhs.a_) && (lhs.b_ == rhs.b_);
    }

private:
    Point a_, b_;
};

class Any : Geometry {
public:
    Any() = default;
    Any(std::unique_ptr<Geometry> ptr) : ptr_(std::move(ptr)) {}

    virtual void accept(Visitor& v) override {

        Geometry::Type type;

        if (ptr_ != nullptr) { type = ptr_->type(); }
        v.visit(type);

        if (ptr_ == nullptr) { ptr_.reset(Visitor::make_geometry(type)); }
        ptr_->accept(v);
    }

    virtual Geometry::Type type() const { return Geometry::Type::ANY; };

    const Geometry* get() { return ptr_.get(); }

private:
    std::unique_ptr<Geometry> ptr_ = nullptr;
};

Geometry* Visitor::make_geometry(Geometry::Type type) {
    switch (type) {
        case Geometry::Type::POINT: return new Point;
        case Geometry::Type::LINE: return new Line;
        default: return nullptr;
    }
};

class Serializer : public Visitor {
public:
    virtual void visit(Geometry::Type& type) { ss_ << size_t(type) << ";"; }
    virtual void visit(long& number) override { ss_ << number << ";"; }
    virtual void visit(Point& item) override { item.accept(*this); }
    virtual void visit(Line& item) override { item.accept(*this); }

    std::string str() const { return ss_.str(); }

private:
    std::stringstream ss_;
};

class Parser : public Visitor {
    public:
        Parser(const std::string& s) { ss_.str(s); }

        virtual void visit(Geometry::Type& type) {
            size_t tmp;
            ss_ >> tmp;
            ss_.ignore(1, ';');
            type = Geometry::Type(tmp);
        }

        virtual void visit(long& number) override {
            ss_ >> number;
            ss_.ignore(1, ';');
        }

        virtual void visit(Point& item) override { item.accept(*this); }
        virtual void visit(Line& item) override { item.accept(*this); }

    private:
        Geometry::Type type_;
        std::stringstream ss_;
    };

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(Visitor, Test01) {
    {
        Point a(1, 2);

        Serializer serivalizer;
        a.accept(serivalizer);
        EXPECT_EQ(serivalizer.str(), "1;2;");

        Point b;
        Parser parser("1;2;");
        b.accept(parser);
        EXPECT_EQ(a, b);
    }
    std::cout << "-- DONE" << std::endl;
}

TEST(Visitor, Test02) {
    {
        Line a(Point(1, 2), Point(3, 4));

        Serializer serivalizer;
        a.accept(serivalizer);
        EXPECT_EQ(serivalizer.str(), "1;2;3;4;");

        Line b;
        Parser parser("1;2;3;4;");
        b.accept(parser);
        EXPECT_EQ(a, b);
    }
    std::cout << "-- DONE" << std::endl;
}

TEST(Visitor, Test03) {
    {
        Point point = Point(1, 2);
        Any a(std::make_unique<Point>(point));

        Serializer serivalizer;
        a.accept(serivalizer);
        EXPECT_EQ(serivalizer.str(), "101;1;2;");

        Any b;
        Parser parser("101;1;2;");
        b.accept(parser);
        EXPECT_EQ(point, *(static_cast<const Point*>(b.get())) ) ;
    }
    std::cout << "-- DONE" << std::endl;
}