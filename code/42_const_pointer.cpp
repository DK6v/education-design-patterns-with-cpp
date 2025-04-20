#include "common.h"

#include "boost/type_index.hpp"

using namespace std;

int main(int argc, char** argv) {

    (void)argc;
    (void)argv;

    cout << "int sz: " << sizeof(int) << endl;
    cout << "int* sz: " << sizeof(int*) << endl;

    int a = 1;
    cout << "int: "
         << boost::typeindex::type_id_runtime(a).pretty_name()
         << endl;

    const int ca1 = 1;
    cout << "const int: "
         << boost::typeindex::type_id_with_cvr<decltype(ca1)>().pretty_name()
         << endl;

    int const ca2 = 1;
    cout << "int const: "
         << boost::typeindex::type_id_with_cvr<decltype(ca2)>().pretty_name()
         << endl;

    const int& ra1 = 1;
    cout << "const int&: "
         << boost::typeindex::type_id_with_cvr<decltype(ra1)>().pretty_name()
         << endl;

    const int* pa1 = &ca1;
    cout << "const int*: "
         << boost::typeindex::type_id_with_cvr<decltype(pa1)>().pretty_name()
         << endl;

    int const* pa2 = &ca1;
    cout << "int const*: "
         << boost::typeindex::type_id_with_cvr<decltype(pa2)>().pretty_name()
         << endl;

    const int* const pa3 = &ca1;
    cout << "const int* const: "
         << boost::typeindex::type_id_with_cvr<decltype(pa3)>().pretty_name()
         << endl;

    int const* const pa4 = &ca1;
    cout << "int const* const: "
         << boost::typeindex::type_id_with_cvr<decltype(pa4)>().pretty_name()
         << endl;
    /*
    int const * const
    |<----->|         == const int

              |<--->| == const *

    const int * const

    |<----->|         == const int

              |<--->| == const *

    */

    return 0;
}