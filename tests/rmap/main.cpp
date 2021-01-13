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
    using namespace spacewire::rmap;
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
        REQUIRE(fields::packet_type(packet) == 0x3);
        REQUIRE(fields::destination_key(packet) == 0x4);
        REQUIRE(fields::source_logical_address(packet) == 0x5);
        REQUIRE(fields::transaction_idetifier(packet) == 0x607);
        REQUIRE(fields::address(packet) == 0x90A0B0C);
        WHEN("setting address")
        {
            fields::address(packet) = 0x11223344;
            THEN("address should be set") { REQUIRE(fields::address(packet) == 0x11223344); }
        }
        REQUIRE(fields::data_length<rmap_write_cmd_tag>(packet) == 0x0D0E0F);
        WHEN("setting data length")
        {
            fields::data_length<rmap_write_cmd_tag>(packet) = 0x112233;
            THEN("data length should be set")
            {
                REQUIRE(fields::data_length<rmap_write_cmd_tag>(packet) == 0x112233);
            }
        }
        REQUIRE(fields::header_crc<rmap_write_cmd_tag>(packet) == 16);
    }
}

SCENARIO("RMAP read request", "[]")
{
    using namespace spacewire::rmap;
    auto packet = build_read_request(254, 2, 32, 0x80000000, 0x1234, 32);
    REQUIRE_THAT(std::vector(packet, packet + read_request_buffer_size()),
        Catch::Equals<uint8_t>({ 0xfe, 0x1, 0x4c, 0x2, 0x20, 0x12, 0x34, 0x0, 0x80, 0x0, 0x0, 0x0,
            0x0, 0x0, 0x20, 0x3a }));
    REQUIRE(header_crc_valid<rmap_read_cmd_tag>(packet));
}
