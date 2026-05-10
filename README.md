# GlowCore Audio Serial Control Protocol

**Specification version 1.1**

This repository contains the specification and reference implementation of a compact USB-based control protocol for GlowCore Audio studio effect devices, including tube compressors, tube saturators, equalizers, preamps, and future hardware units.

---

## 1. Overview

The protocol defines a fixed 32-bit message format for bidirectional control and status monitoring between a host PC and a connected audio hardware device. By standardizing the packet layout and using an extensible `ComponentID` namespace, the same link layer can support multiple device families without changing the packet format.

Supported or reserved parameter categories include:

- **Preamps**: gain, impedance, phantom power, pan.
- **Compressors**: threshold, ratio, attack, release, makeup gain, mode.
- **Saturators**: input gain, mix, output gain, triode/pentode bias controls, pre/post EQ, bypass.
- **Equalizers**: band frequency, gain, and Q controls.
- **Effects**: delay, reverb, wet/dry mix, feedback.
- **Real-time meters**: level, reduction, compression amount, temperature or status values.
- **Service and identification**: device ID, firmware version, host-service channel.

---

## 2. Packet Structure

All messages are exactly **32 bits** / **4 bytes**.

The packet is packed into a `uint32_t`. Bit 0 is the least significant bit of that word. The USB CDC byte order is **big-endian word order**, meaning the host sends the most significant byte first.

```text
┌───────────────────────────────────────────────────────────┐
| 0–2   | ProtocolVersion  | Protocol version              |  3 bits |
| 3–4   | PacketSequence   | Packet sequence number         |  2 bits |
| 5–9   | SystemID         | Host/device/channel ID         |  5 bits |
| 10–17 | ComponentID      | Component/parameter ID         |  8 bits |
| 18–19 | MessageID        | Message type                   |  2 bits |
| 20–27 | MessageValue     | Payload value                  |  8 bits |
| 28–31 | CheckSum         | CRC4 over lower 28 bits        |  4 bits |
└───────────────────────────────────────────────────────────┘
```

The checksum covers the lower 28 bits, from `ProtocolVersion` through `MessageValue`.

### 2.1 Normative bit packing

Host and firmware implementations must use explicit shifts and masks. Do not rely on C bit-field memory layout for serialization.

```c
uint32_t packet = 0;
packet |= ((uint32_t)ProtocolVersion & 0x07U) << 0;
packet |= ((uint32_t)PacketSequence  & 0x03U) << 3;
packet |= ((uint32_t)SystemID        & 0x1FU) << 5;
packet |= ((uint32_t)ComponentID     & 0xFFU) << 10;
packet |= ((uint32_t)MessageID       & 0x03U) << 18;
packet |= ((uint32_t)MessageValue    & 0xFFU) << 20;
packet |= ((uint32_t)CheckSum        & 0x0FU) << 28;
```

The corresponding firmware-side unpacking contract is:

```c
ProtocolVersion = readBits(data, 3, 0);
PacketSequence  = readBits(data, 2, 3);
SystemID        = readBits(data, 5, 5);
ComponentID     = readBits(data, 8, 10);
MessageID       = readBits(data, 2, 18);
MessageValue    = readBits(data, 8, 20);
CheckSum        = readBits(data, 4, 28);
```

### 2.2 USB CDC byte order

The transmitted 4-byte frame is the big-endian representation of the 32-bit packet word.

For example, packet word `0x12345678` is transmitted as:

```text
12 34 56 78
```

Firmware reconstructs it as:

```c
uint32_t receivedData =
    ((uint32_t)RXbuffer[3]) |
    ((uint32_t)RXbuffer[2] << 8) |
    ((uint32_t)RXbuffer[1] << 16) |
    ((uint32_t)RXbuffer[0] << 24);
```

---

## 3. System IDs

`SystemID` identifies the packet destination or origin. Stereo devices expose separate system IDs per channel. The field is **5 bits wide**, so valid values are `0..31`.

| Value | Name                             | Description                                    |
|:-----:|----------------------------------|------------------------------------------------|
| `0`   | `UNDEFINED_SYS_ID`               | Undefined / invalid system                     |
| `1`   | `PC_HOST_ID`                     | Host PC                                        |
| `2`   | `GCA_LEX_MASTERCOM_CH1_ID`       | GlowCore Audio LEX compressor, channel 1       |
| `3`   | `GCA_LEX_MASTERCOM_CH2_ID`       | GlowCore Audio LEX compressor, channel 2       |
| `4`   | `GCA_NASTY_SATURATOR_CH1_ID`     | GlowCore Audio Nasty tube saturator, channel 1 |
| `5`   | `GCA_NASTY_SATURATOR_CH2_ID`     | GlowCore Audio Nasty tube saturator, channel 2 |

---

## 4. Packet Sequence

`PacketSequence` is **2 bits wide**, so valid values are `0..3`.

Recommended working rule:

- use `0` for ordinary single-packet read/write transactions;
- increment modulo 4 only for flows that explicitly need packet sequencing.

---

## 5. Message IDs

`MessageID` is **2 bits wide**, so valid values are `0..3`.

| Value | Enum                 | Description                                      |
|:-----:|----------------------|--------------------------------------------------|
| `0`   | `READ_VALUE_MSG`     | Host requests the current parameter value        |
| `1`   | `SET_VALUE_MSG`      | Host sets a new parameter value                  |
| `2`   | `UPDATE_VALUE_MSG`   | Device sends a live/status value update          |
| `3`   | `RESPONSE`           | Device acknowledges a command or returns a value |

---

## 6. Component IDs

`ComponentID` is an 8-bit parameter namespace. Existing legacy compressor IDs are preserved at the start of the enum. Saturator IDs are assigned explicitly in the `0x80-0x9F` range.

### 6.1 Compressor / legacy control IDs

The current header keeps the existing compressor-style controls:

- `GAIN_0` ... `GAIN_5`
- `THRESHOLD_0` ... `THRESHOLD_5`
- `RATIO_0` ... `RATIO_5`
- `ATTACK_0` ... `ATTACK_5`
- `RELEASE_0` ... `RELEASE_5`
- `MIX_0` ... `MIX_5`
- `VU_MODE_0` ... `VU_MODE_5`
- `COMP_TYPE_0` ... `COMP_TYPE_5`
- `SIDE_CHAIN_0` ... `SIDE_CHAIN_5`
- `LEVEL`, `REDUCTION`, `LEVEL_REDUCTION`

### 6.2 Saturator range: `0x80-0x9F`

| ID     | Name                       | Description                  |
|:------:|----------------------------|------------------------------|
| `0x80` | `SAT_GAIN_IN`              | Saturator input gain         |
| `0x81` | `SAT_MIX`                  | Dry/wet or processed mix     |
| `0x82` | `SAT_GAIN_OUT`             | Saturator output gain        |
| `0x83` | `SAT_TR_BIAS`              | Triode stage bias            |
| `0x84` | `SAT_PNT_BIAS`             | Pentode stage bias           |
| `0x85` | `SAT_TR_PNT_MIX`           | Triode/pentode blend         |
| `0x86` | `SAT_PREEQ_ON`             | Pre-EQ enable                |
| `0x87` | `SAT_PREEQ_BAND_1_GAIN`    | Pre-EQ band 1 gain           |
| `0x88` | `SAT_PREEQ_BAND_2_GAIN`    | Pre-EQ band 2 gain           |
| `0x89` | `SAT_PREEQ_BAND_3_GAIN`    | Pre-EQ band 3 gain           |
| `0x8A` | `SAT_POSTEQ_ON`            | Post-EQ enable               |
| `0x8B` | `SAT_POSTEQ_BAND_1_GAIN`   | Post-EQ band 1 gain          |
| `0x8C` | `SAT_POSTEQ_BAND_2_GAIN`   | Post-EQ band 2 gain          |
| `0x8D` | `SAT_POSTEQ_BAND_3_GAIN`   | Post-EQ band 3 gain          |
| `0x8E` | `SAT_BYPASS`               | Saturator bypass             |

`0x8F-0x9F` are currently unused/reserved for future saturator controls.

### 6.3 Host service range

| ID      | Name          | Description          |
|:-------:|---------------|----------------------|
| `250`   | `HOST_LINE_0` | Service / ACK line   |
| `251`   | `HOST_LINE_1` | Service line         |
| `252`   | `HOST_LINE_2` | Service line         |
| `253`   | `HOST_LINE_3` | Service line         |
| `254`   | `HOST_LINE_4` | Service line         |
| `255`   | `HOST_LINE_5` | Service line         |

---

## 7. MessageValue

`MessageValue` is an unsigned 8-bit value. Its interpretation depends on `ComponentID`:

- Continuous parameters use device-specific scaling over `0-255`.
- Boolean parameters usually use `0 = Off`, `1 = On`.
- Discrete selectors use device-specific enumerated values.
- Response packets may use command-result codes such as `GCA_COMMAND_DONE`, `GCA_COMMAND_FAILURE`, and `GCA_COMMAND_UNSUPPORTED`.

---

## 8. CRC4 Calculation

The checksum is a 4-bit CRC over the lower 28 bits of the packet. It is the same algorithm as the firmware call:

```c
uint8_t checksum = crc4(0, data_without_checksum, 28);
```

Before calculating CRC4, bits `28..31` must be cleared to zero. The CRC input is masked to the lower 28 bits, aligned to a 4-bit boundary, then processed nibble-by-nibble from the most significant nibble to the least significant nibble using polynomial `0b10111`.

A packet with an invalid checksum must be rejected. Current firmware marks such a decoded packet as `UNDEFINED_SYS_ID`.

---

## 9. Typical Command Sequences

### 9.1 Set saturator input gain

Example fields:

| Packet Field     | Example value                    |
|:-----------------|:---------------------------------|
| ProtocolVersion  | `GCA_LINK_VER_0`                 |
| PacketSequence   | `0`                              |
| SystemID         | `GCA_NASTY_SATURATOR_CH1_ID`     |
| ComponentID      | `SAT_GAIN_IN`                    |
| MessageID        | `SET_VALUE_MSG`                  |
| MessageValue     | `10`                             |
| CheckSum         | `8`                              |

Packed word and USB CDC bytes:

```text
word  = 0x80A60080
bytes = 80 A6 00 80
```

### 9.2 Read saturator input gain

| Packet Field     | Example value                    |
|:-----------------|:---------------------------------|
| ProtocolVersion  | `GCA_LINK_VER_0`                 |
| PacketSequence   | `0`                              |
| SystemID         | `GCA_NASTY_SATURATOR_CH1_ID`     |
| ComponentID      | `SAT_GAIN_IN`                    |
| MessageID        | `READ_VALUE_MSG`                 |
| MessageValue     | `0`                              |
| CheckSum         | CRC4 over lower 28 bits          |

### 9.3 ACK response

| Packet Field     | Example value                    |
|:-----------------|:---------------------------------|
| ProtocolVersion  | `GCA_LINK_VER_0`                 |
| PacketSequence   | `0`                              |
| SystemID         | `PC_HOST_ID`                     |
| ComponentID      | `HOST_LINE_0`                    |
| MessageID        | `RESPONSE`                       |
| MessageValue     | `GCA_COMMAND_DONE`               |
| CheckSum         | CRC4 over lower 28 bits          |

---

## 10. Extension Guidelines

When adding a new device or feature:

1. Assign a stable `GCA_SYSTEM_ID` value for the device family/channel.
2. Place new `ComponentID` values into an unused or documented range.
3. Keep existing numeric IDs stable for backward compatibility.
4. Document the `MessageValue` scaling for each new control.
5. If a device receives an unsupported `ComponentID`, respond with `GCA_COMMAND_UNSUPPORTED`.

---

## 11. Implementation Notes

- For single-packet commands, use `PacketSequence = 0`.
- For multi-packet transfers, increment `PacketSequence` modulo 4.
- Host software should wait for a `RESPONSE` after `SET_VALUE_MSG` and `READ_VALUE_MSG` unless a specific streaming mode defines otherwise.
- Real-time `UPDATE_VALUE_MSG` packets may be sent continuously and should be buffered or rate-limited by the host application.
- The C `GCA_PACKET` structure documents logical fields only. It must not be serialized by copying raw struct memory.

---

## License

This protocol specification and reference implementation are released under the **MIT License**.
