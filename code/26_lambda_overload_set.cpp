#include <assert.h>

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>

using namespace std;

template <class ... F> struct overload_set;

template <typename Head>
struct overload_set<Head> : public Head {
    overload_set(Head&& head) : Head(std::move(head)) {}
    overload_set(const Head& head) : Head(head) {}
    using Head::operator();
};

template <typename Head, typename ... Tail>
struct overload_set<Head, Tail ...> : public Head, public overload_set<Tail ...> {
    overload_set(Head&& head, Tail&& ... tail) :
        Head(std::move(head)), overload_set<Tail ...>(std::forward<Tail>(tail) ...) {}
    overload_set(const Head& head, const Tail& ... tail) :
        Head(head), overload_set<Tail ...>(std::forward<Tail>(tail) ...) {}
    using Head::operator();
};

template <typename ... F>
auto overload(F&& ... f) {
    return overload_set<F ...>(std::forward<F>(f) ...);
}

template <typename C>
void print(C& container) {
    cout << "Containter: ";
    for (auto & item: container) {
        cout << item << " ";
    }
    cout << endl;
}

void f1(int)    { std::cout << "f1: int" << endl; }
void f1(float)  { std::cout << "f1: float" << endl; }
void f1(int*)   { std::cout << "f1: int*" << endl; }
void f1(float*) { std::cout << "f1: float*" << endl; }

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    auto compareL1 = [](auto a, auto b) { return a < b; };
    vector<int> l1 = {3, 2, 1};
    print(l1);
    sort(l1.begin(), l1.end(), compareL1);
    print(l1);

    auto overloadCompare = overload(
        [](int a, int b) {
            std::cout << "compare i: " << a << " " << b << endl;
            return a < b;
        },
        [](float a, float b) {
            std::cout << "compare f: " << a << " " << b << endl;
            return a < b;
        }
    );

    vector<int> l2 = {6, 5, 4};
    print(l2);
    sort(l2.begin(), l2.end(), overloadCompare);
    print(l2);

    vector<float> l3 = {9.1, 8.1, 7.1};
    print(l3);
    sort(l3.begin(), l3.end(), overloadCompare);
    print(l3);

    auto overloadEcho = overload(
        [](int a) { std::cout << "overload i: " << a <<  endl; },
        [](float a) { std::cout << "overload f: " << a <<  endl; },
        [](int* a) { std::cout << "overload i*: " << a <<  endl; },
        [](float* a) { std::cout << "overload f*: " << a << endl; },
        [](string a) { std::cout << "overload s: " << a.c_str() << endl; }
    );

    int a = 1;
    float b = 1.1;

    overloadEcho.operator()(a);
    overloadEcho.operator()(static_cast<float>(b)); // <== FAIL, see target 27
    overloadEcho(&a);
    overloadEcho(&b);
    overloadEcho("HELLO");

    f1(a);
    f1(b); // <== CORRECT
    f1(&a);
    f1(&b);

    return 0;
}