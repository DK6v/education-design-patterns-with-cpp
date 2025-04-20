#include "boost/type_index.hpp"
#include <gtest/gtest.h>

template<typename Lambda>
struct is_valid_helper {

    template <typename... LambdaArgs>
    constexpr auto test(int) ->
        decltype(std::declval<Lambda>()(std::declval<LambdaArgs>()...),
                 std::true_type()) {
        return std::true_type();
    }

    template <typename... LambdaArgs>
    constexpr auto test(...) {
        return std::false_type();
    }

    template <typename... LambdaArgs>
    constexpr auto operator()(const LambdaArgs& ...) {
        return this->test<LambdaArgs...>(0);
    }
};

template <typename Lambda>
constexpr auto is_valid(const Lambda&) {
    return is_valid_helper<Lambda>();
}

auto is_pointer = is_valid([](auto && x) -> decltype(*x) {});

int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

TEST(FunctionOverloadIsValid, Test1) {
    const int i = 1;
    const int* pi = &i;

    EXPECT_EQ(false, decltype(is_pointer(i))::value);
    EXPECT_EQ(true, decltype(is_pointer(pi))::value);
}
