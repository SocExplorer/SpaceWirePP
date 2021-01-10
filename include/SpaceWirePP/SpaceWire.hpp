/*------------------------------------------------------------------------------
--  This file is a part of the SpaceWire++ Library
--  Copyright (C) 2021, Plasma Physics Laboratory - CNRS
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 2 of the License, or
--  (at your option) any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the Free Software
--  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
-------------------------------------------------------------------------------*/
/*--                  Author : Alexis Jeandet
--                     Mail : alexis.jeandet@lpp.polytechnique.fr
----------------------------------------------------------------------------*/
#pragma once
#include "endianness.hpp"
#include <array>
#include <iostream>
#include <numeric>

namespace spacewire
{

struct uint24_t
{
    uint32_t value;
    uint24_t(const int& v) : value { static_cast<uint32_t>(v) } { }
    operator uint32_t() { return value; }
    uint24_t& operator=(const uint32_t& v)
    {
        value = v;
        return *this;
    }
    uint24_t& operator=(const int& v)
    {
        value = v;
        return *this;
    }
    bool operator==(const uint24_t& other) const { return other.value == value; }

    bool operator==(const int& v) const { return static_cast<uint32_t>(v) == value; }
};

template <typename T, bool is_const=false>
struct field_proxy
{
    using packet_type = std::conditional_t<is_const,const unsigned char*,unsigned char*>;
    field_proxy(packet_type packet) : packet { packet } { }
    field_proxy(const field_proxy& other) : packet { other.packet } { }
    field_proxy(field_proxy&& other) : packet { other.packet } { }

    bool operator==(const T& value) const { return T(*this) == value; }

    bool operator==(const int& value) const { return this->operator T() == static_cast<T>(value); }

    bool operator==(const field_proxy<T>& other) const { return T(*this) == T(other); }

    field_proxy& operator=(const T& value)
    {
        if constexpr (std::is_same_v<uint24_t, T>)
        {
            if constexpr (Endianness::is_big_endian_v<Endianness::host_endianness_t>)
            {
                std::memcpy(packet, &value.value, 3);
            }
            else
            {
                uint32_t copy = 0x00FFFFFF & value.value;
                copy = Endianness::details::bswap(copy);
                std::memcpy(packet, reinterpret_cast<char*>(&copy) + 1, 3);
            }
        }
        else
        {
            Endianness::encode<Endianness::big_endian_t>(&value, 1, packet);
        }
        return *this;
    }

    std::ostream& operator<<(std::ostream& os)
    {
        os << this->operator T();
        return os;
    }

    template <typename Dummy = void,
        typename = std::enable_if_t<std::is_same_v<uint24_t, T>, Dummy>>
    operator uint32_t() const
    {
        return this->operator T().value;
    }

    operator T() const
    {
        if constexpr (std::is_same_v<uint24_t, T>)
        {
            if constexpr (Endianness::is_big_endian_v<Endianness::host_endianness_t>)
            {
                T value { 0U };
                std::memcpy(&value.value, packet, 3);
                return value;
            }
            else
            {
                uint32_t value { 0U };
                std::memcpy(reinterpret_cast<char*>((&value)) + 1, packet, 3);
                value = Endianness::details::bswap(value);
                return static_cast<T>(value);
            }
        }
        else
        {
            return Endianness::decode<Endianness::big_endian_t, T>(packet);
        }
    }

private:
    packet_type packet;
};

enum class protocol_id_t:unsigned char
{
    SPW_PROTO_ID_EXTEND = 0,
    SPW_PROTO_ID_RMAP = 1,
    SPW_PROTO_ID_CCSDS = 2,
    SPW_PROTO_ID_GOES_R = 238,
    SPW_PROTO_ID_STUP = 239
};

static constexpr std::array<unsigned char, 256> CRCTable { 0x00, 0x91, 0xe3, 0x72, 0x07, 0x96, 0xe4,
    0x75, 0x0e, 0x9f, 0xed, 0x7c, 0x09, 0x98, 0xea, 0x7b, 0x1c, 0x8d, 0xff, 0x6e, 0x1b, 0x8a, 0xf8,
    0x69, 0x12, 0x83, 0xf1, 0x60, 0x15, 0x84, 0xf6, 0x67, 0x38, 0xa9, 0xdb, 0x4a, 0x3f, 0xae, 0xdc,
    0x4d, 0x36, 0xa7, 0xd5, 0x44, 0x31, 0xa0, 0xd2, 0x43, 0x24, 0xb5, 0xc7, 0x56, 0x23, 0xb2, 0xc0,
    0x51, 0x2a, 0xbb, 0xc9, 0x58, 0x2d, 0xbc, 0xce, 0x5f, 0x70, 0xe1, 0x93, 0x02, 0x77, 0xe6, 0x94,
    0x05, 0x7e, 0xef, 0x9d, 0x0c, 0x79, 0xe8, 0x9a, 0x0b, 0x6c, 0xfd, 0x8f, 0x1e, 0x6b, 0xfa, 0x88,
    0x19, 0x62, 0xf3, 0x81, 0x10, 0x65, 0xf4, 0x86, 0x17, 0x48, 0xd9, 0xab, 0x3a, 0x4f, 0xde, 0xac,
    0x3d, 0x46, 0xd7, 0xa5, 0x34, 0x41, 0xd0, 0xa2, 0x33, 0x54, 0xc5, 0xb7, 0x26, 0x53, 0xc2, 0xb0,
    0x21, 0x5a, 0xcb, 0xb9, 0x28, 0x5d, 0xcc, 0xbe, 0x2f, 0xe0, 0x71, 0x03, 0x92, 0xe7, 0x76, 0x04,
    0x95, 0xee, 0x7f, 0x0d, 0x9c, 0xe9, 0x78, 0x0a, 0x9b, 0xfc, 0x6d, 0x1f, 0x8e, 0xfb, 0x6a, 0x18,
    0x89, 0xf2, 0x63, 0x11, 0x80, 0xf5, 0x64, 0x16, 0x87, 0xd8, 0x49, 0x3b, 0xaa, 0xdf, 0x4e, 0x3c,
    0xad, 0xd6, 0x47, 0x35, 0xa4, 0xd1, 0x40, 0x32, 0xa3, 0xc4, 0x55, 0x27, 0xb6, 0xc3, 0x52, 0x20,
    0xb1, 0xca, 0x5b, 0x29, 0xb8, 0xcd, 0x5c, 0x2e, 0xbf, 0x90, 0x01, 0x73, 0xe2, 0x97, 0x06, 0x74,
    0xe5, 0x9e, 0x0f, 0x7d, 0xec, 0x99, 0x08, 0x7a, 0xeb, 0x8c, 0x1d, 0x6f, 0xfe, 0x8b, 0x1a, 0x68,
    0xf9, 0x82, 0x13, 0x61, 0xf0, 0x85, 0x14, 0x66, 0xf7, 0xa8, 0x39, 0x4b, 0xda, 0xaf, 0x3e, 0x4c,
    0xdd, 0xa6, 0x37, 0x45, 0xd4, 0xa1, 0x30, 0x42, 0xd3, 0xb4, 0x25, 0x57, 0xc6, 0xb3, 0x22, 0x50,
    0xc1, 0xba, 0x2b, 0x59, 0xc8, 0xbd, 0x2c, 0x5e, 0xcf };


inline unsigned char crc(unsigned char* buffer, int size)
{
    return std::accumulate(buffer, buffer+size, 0,[](auto crc, auto value){return CRCTable[crc ^ value];});
}

namespace fields
{
    inline const unsigned char& destination_logical_address(const unsigned char* packet) { return packet[0]; }
    inline unsigned char& destination_logical_address(unsigned char* packet) { return packet[0]; }
    inline field_proxy<protocol_id_t> protocol_identifier(unsigned char* packet) { return {packet+1}; }
    inline field_proxy<protocol_id_t, true> protocol_identifier(const unsigned char* packet) { return {packet+1}; }
}

}
