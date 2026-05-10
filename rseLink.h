#ifndef GLOWCORE_LINK_RSELINK_H
#define GLOWCORE_LINK_RSELINK_H

#include <stdint.h>

/*
 * GlowCore Link packet layout, packed into a uint32_t.
 * Bit 0 is the least significant bit of the 32-bit word.
 * USB CDC byte order is big-endian word order: byte[0] is bits 31..24.
 *
 * Bits  0..2   ProtocolVersion   3 bits
 * Bits  3..4   PacketSequence    2 bits
 * Bits  5..9   SystemID          5 bits
 * Bits 10..17  ComponentID       8 bits
 * Bits 18..19  MessageID         2 bits
 * Bits 20..27  MessageValue      8 bits
 * Bits 28..31  CheckSum          4 bits, crc4(0, packet_without_checksum, 28)
 */

#define GCA_PROTOCOL_VERSION_POS   0U
#define GCA_PROTOCOL_VERSION_SIZE  3U
#define GCA_PACKET_SEQUENCE_POS    3U
#define GCA_PACKET_SEQUENCE_SIZE   2U
#define GCA_SYSTEM_ID_POS          5U
#define GCA_SYSTEM_ID_SIZE         5U
#define GCA_COMPONENT_ID_POS       10U
#define GCA_COMPONENT_ID_SIZE      8U
#define GCA_MESSAGE_ID_POS         18U
#define GCA_MESSAGE_ID_SIZE        2U
#define GCA_MESSAGE_VALUE_POS      20U
#define GCA_MESSAGE_VALUE_SIZE     8U
#define GCA_CHECKSUM_POS           28U
#define GCA_CHECKSUM_SIZE          4U

typedef enum {
	GCA_LINK_VER_0 = 0
} GCA_LINK_VER;

typedef enum {
	MESSAGE_IS_EMPTY = 0,
	MESSAGE_NOT_EMPTY
} GCA_MESSAGE_EMPTY;


typedef enum {
	GCA_COMMAND_DONE = 0,
	GCA_COMMAND_FAILURE,
	GCA_COMMAND_UNSUPPORTED
} GCA_COMMAND_RESULT;


// SYSTEM IDs begin
typedef enum {
	UNDEFINED_SYS_ID = 0,
	PC_HOST_ID,
	GCA_LEX_MASTERCOM_CH1_ID,
	GCA_LEX_MASTERCOM_CH2_ID,
	GCA_NASTY_SATURATOR_CH1_ID,
	GCA_NASTY_SATURATOR_CH2_ID
} GCA_SYSTEM_ID;


// COMPONENT IDs begin
typedef enum {
	GAIN_0 = 0,
	GAIN_1,
	GAIN_2,
	GAIN_3,
	GAIN_4,
	GAIN_5,

	THRESHOLD_0,
	THRESHOLD_1,
	THRESHOLD_2,
	THRESHOLD_3,
	THRESHOLD_4,
	THRESHOLD_5,

	RATIO_0,
	RATIO_1,
	RATIO_2,
	RATIO_3,
	RATIO_4,
	RATIO_5,

	ATTACK_0,
	ATTACK_1,
	ATTACK_2,
	ATTACK_3,
	ATTACK_4,
	ATTACK_5,

	RELEASE_0,
	RELEASE_1,
	RELEASE_2,
	RELEASE_3,
	RELEASE_4,
	RELEASE_5,

	MIX_0,
	MIX_1,
	MIX_2,
	MIX_3,
	MIX_4,
	MIX_5,

	VU_MODE_0,
	VU_MODE_1,
	VU_MODE_2,
	VU_MODE_3,
	VU_MODE_4,
	VU_MODE_5,

	COMP_TYPE_0,
	COMP_TYPE_1,
	COMP_TYPE_2,
	COMP_TYPE_3,
	COMP_TYPE_4,
	COMP_TYPE_5,

	SIDE_CHAIN_0,
	SIDE_CHAIN_1,
	SIDE_CHAIN_2,
	SIDE_CHAIN_3,
	SIDE_CHAIN_4,
	SIDE_CHAIN_5,

	LEVEL,          // for request to be displayed on VU meter
	REDUCTION,      // for request to be displayed on VU meter
	LEVEL_REDUCTION,// for request to be displayed on VU meter

	// Saturator range: 0x80-0x9F
	SAT_GAIN_IN = 0x80,
	SAT_MIX,
	SAT_GAIN_OUT,
	SAT_TR_BIAS,
	SAT_PNT_BIAS,
	SAT_TR_PNT_MIX,
	SAT_PREEQ_ON,
	SAT_PREEQ_BAND_1_GAIN,
	SAT_PREEQ_BAND_2_GAIN,
	SAT_PREEQ_BAND_3_GAIN,
	SAT_POSTEQ_ON,
	SAT_POSTEQ_BAND_1_GAIN,
	SAT_POSTEQ_BAND_2_GAIN,
	SAT_POSTEQ_BAND_3_GAIN,
	SAT_BYPASS,

	HOST_LINE_0 = 250,
	HOST_LINE_1,
	HOST_LINE_2,
	HOST_LINE_3,
	HOST_LINE_4,
	HOST_LINE_5
} GCA_COMPONENT_ID;


typedef enum {
	READ_VALUE_MSG = 0,
	SET_VALUE_MSG,
	UPDATE_VALUE_MSG,
	RESPONSE,
} GCA_MSG_TYPE_ID;

typedef struct packet
{
	GCA_LINK_VER ProtocolVersion;      // 3 bits, bits 0..2
	uint8_t PacketSequence;            // 2 bits, bits 3..4
	GCA_SYSTEM_ID SystemID;            // 5 bits, bits 5..9, destination system/channel
	GCA_COMPONENT_ID ComponentID;      // 8 bits, bits 10..17, destination component/channel
	GCA_MSG_TYPE_ID MessageID;         // 2 bits, bits 18..19, type of message
	uint8_t MessageValue;              // 8 bits, bits 20..27, desired/respond value
	uint8_t CheckSum;                  // 4 bits, bits 28..31, CRC4 over lower 28 bits
} GCA_PACKET;


GCA_PACKET GCA_link_processInputData(uint32_t);
uint32_t GCA_link_processOutputMessage(GCA_PACKET packet);
uint8_t GCA_link_respondResult(uint8_t sucess);
uint8_t GCA_link_respondValue(uint8_t channel, uint8_t value);

#endif // GLOWCORE_LINK_RSELINK_H
