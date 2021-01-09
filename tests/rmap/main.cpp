#define CATCH_CONFIG_MAIN
#if __has_include(<catch2/catch.hpp>)
#include <catch2/catch.hpp>
#include <catch2/catch_reporter_tap.hpp>
#include <catch2/catch_reporter_teamcity.hpp>
#else
#include <catch.hpp>
#include <catch_reporter_tap.hpp>
#include <catch_reporter_teamcity.hpp>
#endif
#include <SpaceWirePP/rmap.hpp>
#include <cstdint>


SCENARIO("RMAP field extraction", "[]")
{
    GIVEN("An RMAP packet")
    {
        unsigned char packet[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
        REQUIRE(spacewire::fields::destination_logical_address(packet) == 0x1);
        WHEN("setting destination logical address")
        {
            spacewire::fields::destination_logical_address(packet) = 0;
            THEN("destination logical address should be set")
            {
                REQUIRE(spacewire::fields::destination_logical_address(packet) == 0x0);
            }
        }
        REQUIRE(spacewire::fields::protocol_identifier(packet) == 0x2);
        REQUIRE(spacewire::rmap::fields::packet_type(packet) == 0x3);
        REQUIRE(spacewire::rmap::fields::destination_key(packet) == 0x4);
        REQUIRE(spacewire::rmap::fields::source_logical_address(packet) == 0x5);
        REQUIRE(spacewire::rmap::fields::transaction_idetifier(packet) == 0x607);
        REQUIRE(spacewire::rmap::fields::address(packet) == 0x90A0B0C);
        WHEN("setting address")
        {
            spacewire::rmap::fields::address(packet) = 0x11223344;
            THEN("address should be set")
            {
                REQUIRE(spacewire::rmap::fields::address(packet) == 0x11223344);
            }
        }
        REQUIRE(spacewire::rmap::fields::data_length(packet) == 0x0D0E0F);
        WHEN("setting data length")
        {
            spacewire::rmap::fields::data_length(packet) = 0x112233;
            THEN("data length should be set")
            {
                REQUIRE(spacewire::rmap::fields::data_length(packet) == 0x112233);
            }
        }
        REQUIRE(spacewire::rmap::fields::header_crc(packet) == 16);
    }
}
