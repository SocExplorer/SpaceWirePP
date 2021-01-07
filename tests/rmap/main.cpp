#define CATCH_CONFIG_MAIN
#if __has_include(<catch2/catch.hpp>)
#include <catch2/catch.hpp>
#include <catch2/catch_reporter_teamcity.hpp>
#include <catch2/catch_reporter_tap.hpp>
#else
#include <catch.hpp>
#include <catch_reporter_teamcity.hpp>
#include <catch_reporter_tap.hpp>
#endif
#include <cstdint>
#include <SpaceWirePP/SpaceWire.hpp>


TEST_CASE("RMAP", "[]")
{

}
