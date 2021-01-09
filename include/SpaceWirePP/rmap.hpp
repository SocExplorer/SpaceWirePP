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

/*
 *   Rmap Write command header:
 *
 *  | Destination Logical Address | Protocol identifier    | Packet type            | Destination
 * key        |
 *  |-----------------------------|------------------------|------------------------|------------------------|
 *  | Source Logical Address      | Transaction identifier | Transaction identifier | Extended write
 * address |
 *  |-----------------------------|------------------------|------------------------|------------------------|
 *  | Write address MSB           | Write  address         | Write  address         | Write  address
 * LSB     |
 *  |-----------------------------|------------------------|------------------------|------------------------|
 *  | Data length MSB             | Data length            | Data length LSB        | Header CRC |
 *  |-----------------------------|------------------------|------------------------|------------------------|
 *  | Data                        | (...)                  | Last data byte         | Data CRC |
 *  |-----------------------------|------------------------|------------------------|------------------------|
 *
 * Packet type field:
 *
 *  | msb
 *  | reserved = 0 | Comand = 1| Write = 1 |       Verify data = 1 |   Ack  = 1  | Increment/      |
 * Source Path    | Source Path    | | Don't Verify data = 0 | No Ack = 0  | No Inc. address |
 * Address Length | Address Length |
 *
 */

/*
 *   Rmap read command header:
 *
 *  | Destination Logical Address | Protocol identifier    | Packet type            | Destination
 * key        |
 *  |-----------------------------|------------------------|------------------------|------------------------|
 *  | Source Logical Address      | Transaction identifier | Transaction identifier | Extended read
 * address  |
 *  |-----------------------------|------------------------|------------------------|------------------------|
 *  | Read  address MSB           | Read  address          | Read  address          | Read  address
 * LSB      |
 *  |-----------------------------|------------------------|------------------------|------------------------|
 *  | Data length MSB             | Data length            | Data length LSB        | CRC |
 *  |-----------------------------|------------------------|------------------------|------------------------|
 *
 * Packet type field:
 *
 *  | msb
 *  | reserved = 0 | Comand = 1| Read = 0 | Read = 0 | Read = 1    | Increment/      | Source Path
 * | Source Path    | | (Ack/NoAck) | No Inc. address | Address Length | Address Length |
 *
 */

#include "SpaceWire.hpp"
#include <cassert>

namespace spacewire::rmap
{

namespace fields
{
    inline unsigned char& packet_type(unsigned char* packet) { return packet[2]; }

    inline unsigned char& destination_key(unsigned char* packet) { return packet[3]; }

    inline unsigned char& source_logical_address(unsigned char* packet) { return packet[4]; }

    inline field_proxy<uint16_t> transaction_idetifier(unsigned char* packet)
    {
        return field_proxy<uint16_t> { packet + 5 };
    }

    inline unsigned char& extended_read_address(unsigned char* packet) { return packet[7]; }

    inline field_proxy<uint32_t> address(unsigned char* packet)
    {
        return field_proxy<uint32_t> { packet + 8 };
    }

    inline field_proxy<uint24_t> data_length(unsigned char* packet)
    {
        return field_proxy<uint24_t> { packet + 12 };
    }

    inline unsigned char& header_crc(unsigned char* packet) { return packet[15]; }
    inline unsigned char& data_crc(unsigned char* packet)
    {
        std::size_t crc_offset = data_length(packet) + 16;
        return packet[crc_offset];
    }
    inline unsigned char* data(unsigned char* packet) { return packet + 16; }


}


inline constexpr std::size_t request_header_size()
{
    return 16;
}
inline constexpr std::size_t read_request_buffer_size()
{
    return request_header_size();
}
inline constexpr std::size_t write_request_buffer_size(std::size_t data_size)
{
    return request_header_size() + data_size + 1;
}

inline unsigned char* build_read_request(unsigned char destination_logical_address,
    unsigned char destination_key, unsigned char source_logical_address, uint32_t read_address,
    uint16_t transaction_id, uint32_t data_length, unsigned char* buffer = nullptr)
{
    assert(data_length < (1 << 24));
    if (buffer == nullptr)
        buffer = new unsigned char[read_request_buffer_size()]();
    spacewire::fields::destination_logical_address(buffer) = destination_logical_address;
    spacewire::fields::protocol_identifier(buffer) = protocol_id_t::SPW_PROTO_ID_RMAP;
    fields::packet_type(buffer) = 0b01001100;
    fields::destination_key(buffer) = destination_key;
    fields::source_logical_address(buffer) = source_logical_address;
    fields::transaction_idetifier(buffer) = transaction_id;
    fields::extended_read_address(buffer) = 0;
    fields::address(buffer) = read_address;
    fields::data_length(buffer) = data_length;
    fields::header_crc(buffer) = spacewire::crc(buffer, request_header_size()-1);
    return buffer;
}

inline unsigned char* build_write_request(unsigned char destination_logical_address,
    unsigned char destination_key, unsigned char source_logical_address, uint32_t write_address,
    uint16_t transaction_id, unsigned char* data, uint32_t data_length,
    unsigned char* buffer = nullptr)
{
    assert(data_length < (1 << 24));
    if (buffer == nullptr)
        buffer = new unsigned char[write_request_buffer_size(data_length)]();
    spacewire::fields::destination_logical_address(buffer) = destination_logical_address;
    spacewire::fields::protocol_identifier(buffer) = protocol_id_t::SPW_PROTO_ID_RMAP;
    fields::packet_type(buffer) = 0b01001100;
    fields::destination_key(buffer) = destination_key;
    fields::source_logical_address(buffer) = source_logical_address;
    fields::transaction_idetifier(buffer) = transaction_id;
    fields::extended_read_address(buffer) = 0;
    fields::address(buffer) = write_address;
    fields::data_length(buffer) = data_length;
    fields::header_crc(buffer) = spacewire::crc(buffer, request_header_size()-1);
    std::memcpy(fields::data(buffer), data, data_length);
    fields::data_crc(buffer) = spacewire::crc(fields::data(buffer), data_length);
    return buffer;
}

}
