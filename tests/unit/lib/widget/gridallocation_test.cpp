#include "lib/widget/gridallocation.h"
#include <iostream>

#include <catch2/catch.hpp>

TEST_CASE("empty", "[gridallocation]")
{
    grid_allocation::allocator allocator({});
    REQUIRE(allocator.calculate_offsets(1000).size() == 0);
    REQUIRE(allocator.get_minimum_size() == 0);
}

TEST_CASE("single expandable item", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{1, 2, true}, 50}
    });

    REQUIRE(allocator.get_minimum_size() == 50);
    auto offsets = allocator.calculate_offsets(1000);
    REQUIRE(offsets.size() == 2);
    REQUIRE(offsets[1] == 0);
    REQUIRE(offsets[3] == 1000);
}

TEST_CASE("shrinked contiguous", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{0, 1, false}, 50},
        {{1, 1, true}, 30}
    });

    REQUIRE(allocator.get_minimum_size() == 80);
    auto offsets = allocator.calculate_offsets(40);
    REQUIRE(offsets.size() == 3);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[1] == 25);
    REQUIRE(offsets[2] == 40);
}

TEST_CASE("shrinked overlapping", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{0, 2, false}, 50},
        {{1, 2, true}, 30}
    });

    REQUIRE(allocator.get_minimum_size() == 50);
    auto offsets = allocator.calculate_offsets(25);
    REQUIRE(offsets.size() == 4);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[1] == 10);
    REQUIRE(offsets[2] == 25);
    REQUIRE(offsets[3] == 25);
}

TEST_CASE("expanded contiguous", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{0, 1, false}, 50},
        {{1, 1, true}, 30}
    });

    REQUIRE(allocator.get_minimum_size() == 80);
    auto offsets = allocator.calculate_offsets(110);
    REQUIRE(offsets.size() == 3);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[1] == 50);
    REQUIRE(offsets[2] == 110);
}

TEST_CASE("expanded overlapping", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{0, 2, false}, 50},
        {{0, 1, false}, 30},
        {{1, 2, true}, 30}
    });

    REQUIRE(allocator.get_minimum_size() == 60);
    auto offsets = allocator.calculate_offsets(110);
    REQUIRE(offsets.size() == 4);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[1] == 30);
    REQUIRE(offsets[2] == 50);
    REQUIRE(offsets[3] == 110);
}

TEST_CASE("rigid end exact fit", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{0, 2, true}, 50},
        {{1, 1, false}, 40}
    });

    REQUIRE(allocator.get_minimum_size() == 50);
    auto offsets = allocator.calculate_offsets(allocator.get_minimum_size());
    REQUIRE(offsets.size() == 3);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[1] == 10);
    REQUIRE(offsets[2] == 50);
}

TEST_CASE("rigid end expanded", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{0, 2, true}, 50},
        {{1, 1, false}, 40}
    });

    REQUIRE(allocator.get_minimum_size() == 50);
    auto offsets = allocator.calculate_offsets(150);
    REQUIRE(offsets.size() == 3);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[1] == 110);
    REQUIRE(offsets[2] == 150);
}

TEST_CASE("double rigid end", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{0, 3, true}, 50},
        {{1, 1, false}, 10},
        {{2, 1, false}, 20}
    });

    REQUIRE(allocator.get_minimum_size() == 50);
    auto offsets = allocator.calculate_offsets(100);
    REQUIRE(offsets.size() == 4);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[1] == 70);
    REQUIRE(offsets[2] == 80);
    REQUIRE(offsets[3] == 100);
}

TEST_CASE("rigid within two expandables", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{0, 1, true}, 20},
        {{1, 1, false}, 10},
        {{2, 1, true}, 20}
    });

    REQUIRE(allocator.get_minimum_size() == 50);
    auto offsets = allocator.calculate_offsets(100);
    REQUIRE(offsets.size() == 4);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[1] == 45);
    REQUIRE(offsets[2] == 55);
    REQUIRE(offsets[3] == 100);
}

TEST_CASE("rigid with different sizes", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{1, 1, false}, 30},
        {{1, 1, false}, 20}
    });

    REQUIRE(allocator.get_minimum_size() == 30);
    auto offsets = allocator.calculate_offsets(30);
    REQUIRE(offsets.size() == 2);
    REQUIRE(offsets[1] == 0);
    REQUIRE(offsets[2] == 30);
}

TEST_CASE("discontiguous rigids with different sizes", "[gridallocation]")
{
    grid_allocation::allocator allocator({
        {{0, 2, false}, 30},
        {{1, 2, false}, 20}
    });

    REQUIRE(allocator.get_minimum_size() == 30);
    auto offsets = allocator.calculate_offsets(30);
    REQUIRE(offsets.size() == 4);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[1] == 10);
    REQUIRE(offsets[2] == 30);
    REQUIRE(offsets[3] == 30);
}

//*
static std::vector<grid_allocation::item> mixedItems()
{
    return {
        {{0, 6, true}, 40},
        {{0, 2, false}, 50},
        {{2, 2, true}, 150},
        {{4, 2, false}, 60},
        {{0, 3, true}, 200},
        {{3, 3, true}, 200}
    };
}

TEST_CASE("mixed items exact fit", "[gridallocation]")
{
    grid_allocation::allocator allocator(mixedItems());

    REQUIRE(allocator.get_minimum_size() == 400);
    auto offsets = allocator.calculate_offsets(allocator.get_minimum_size());
    REQUIRE(offsets.size() == 5);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[2] == 50);
    REQUIRE(offsets[3] == 200);
    REQUIRE(offsets[4] == 340);
    REQUIRE(offsets[6] == 400);
}

TEST_CASE("mixed items expanded", "[gridallocation]")
{
    grid_allocation::allocator allocator(mixedItems());

    REQUIRE(allocator.get_minimum_size() == 400);
    auto offsets = allocator.calculate_offsets(500);
    REQUIRE(offsets.size() == 5);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[2] == 50);
    REQUIRE(offsets[3] == 250);
    REQUIRE(offsets[4] == 440);
    REQUIRE(offsets[6] == 500);
}

TEST_CASE("mixed items shrinked", "[gridallocation]")
{
    grid_allocation::allocator allocator(mixedItems());

    REQUIRE(allocator.get_minimum_size() == 400);
    auto offsets = allocator.calculate_offsets(200);
    REQUIRE(offsets.size() == 5);
    REQUIRE(offsets[0] == 0);
    REQUIRE(offsets[2] == 25);
    REQUIRE(offsets[3] == 100);
    REQUIRE(offsets[4] == 170);
    REQUIRE(offsets[6] == 200);
}
//*/