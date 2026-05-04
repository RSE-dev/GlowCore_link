typedef enum {
	RSE_LINK_VER_0 =  0
}RSE_LINK_VER;

typedef enum {
MESSAGE_IS_EMPTY = 0,
MESSAGE_NOT_EMPTY
}RSE_MESSAGE_EMPTY;


typedef enum{
RSE_COMMAND_DONE = 0,
RSE_COMMAND_FAILURE,
RSE_COMMAND_UNSUPPORTED
}RSE_COMMAND_RESULT;


//SYSTEMS IDs begin
typedef enum{
UNDEFINED_SYS_ID = 0,
PC_HOST_ID,
GCA_LEX_MASTERCOM_ID,
GCA_NASTY_SATURATOR_ID
}RSE_SYSTEM_ID;


//COMPONENT IDs begin
typedef enum{
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

LEVEL,				//for request to be displayed on VU meter
REDUCTION,			//for request to be displayed on VU meter
LEVEL_REDUCTION,	//for request to be displayed on VU meter


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
}RSE_COMPONENT_ID;


typedef enum{
READ_VALUE_MSG = 0,
SET_VALUE_MSG,
UPDATE_VALUE_MSG,
RESPONSE,
}RSE_MSG_TYPE_ID;

typedef struct packet
{
	RSE_LINK_VER ProtocolVersion; //3 bit Version of the protocol
	uint8_t PacketSequence; //2 bit - 0 if it's the first packet
	RSE_SYSTEM_ID SystemID; //5 bit - destination system
	RSE_COMPONENT_ID ComponentID; //8 bit - destination component/channel
	RSE_MSG_TYPE_ID MessageID; //2 bit - type of message
	uint8_t MessageValue; //8 bit - desired value/respond value
	uint8_t CheckSum; //4bit - CRC4 to check the packet
}RSE_PACKET;


RSE_PACKET RSE_link_processInputData(uint32_t);
uint32_t RSE_link_processOutputMessage(RSE_PACKET packet);
uint8_t RSE_link_respondResult(uint8_t sucess);
uint8_t RSE_link_respondValue(uint8_t channel, uint8_t value);
