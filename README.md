# GlowCore Audio Serial Control Protocol

**Version 1.0**

This repository contains the specification and reference implementation of a compact USB-based control protocol for GlowCore Audio studio effect devices, including tube compressors, tube saturators, equalizers, preamps, and future hardware units.

---

## 1. Overview

The protocol defines a fixed 32-bit message format for bidirectional control and status monitoring between a host PC and a connected audio hardware device. By standardizing the packet layout and using an extensible `ComponentID` namespace, the same link layer can support multiple device families without changing the packet format.

Supported or reserved parameter categories include:

- **Preamps**: gain, impedance, phantom power, pan.
- **Compressors**: threshold, ratio, attack, release, makeup gain, mode.
- **Saturators**: input gain, mix, output gain, tube/transistor bias controls, pre/post EQ, bypass.
- **Equalizers**: band frequency, gain, and Q controls.
- **Effects**: delay, reverb, wet/dry mix, feedback.
- **Real-time meters**: level, reduction, compression amount, temperature or status values.
- **Service and identification**: device ID, firmware version, host-service channel.

---

## 2. Packet Structure

All messages are exactly **32 bits** / **4 bytes**.

```text
┌───────────────────────────────────────────────────────────┐
| 0–2   | ProtocolVersion  | Protocol version              |  3 bits |
| 3–4   | SystemID         | Host/device ID                 |  2 bits |
| 5–9   | PacketSequence   | Packet sequence number         |  5 bits |
| 10–17 | ComponentID      | Component/parameter ID         |  8 bits |
| 18–19 | MessageID        | Message type                   |  2 bits |
| 20–27 | MessageValue     | Payload value                  |  8 bits |
| 28–31 | CheckSum         | CRC4 over lower 28 bits        |  4 bits |
└───────────────────────────────────────────────────────────┘
```

The checksum covers the lower 28 bits, from `ProtocolVersion` through `MessageValue`.

---

## 3. System IDs

`SystemID` identifies the packet destination or origin.

| Value | Name                      | Description                         |
|:-----:|---------------------------|-------------------------------------|
| `0`   | `UNDEFINED_SYS_ID`        | Undefined / invalid system          |
| `1`   | `PC_HOST_ID`              | Host PC                             |
| `2`   | `GCA_LEX_MASTERCOM_ID`    | GlowCore Audio LEX compressor       |
| `3`   | `GCA_NASTY_SATURATOR_ID`  | GlowCore Audio Nasty tube saturator |

Current C definition:

```c
typedef enum{
UNDEFINED_SYS_ID = 0,
PC_HOST_ID,
GCA_LEX_MASTERCOM_ID,
GCA_NASTY_SATURATOR_ID
}GCA_SYSTEM_ID;
```

---

## 4. Message IDs

| Value | Enum                 | Description                                      |
|:-----:|----------------------|--------------------------------------------------|
| `0`   | `READ_VALUE_MSG`     | Host requests the current parameter value        |
| `1`   | `SET_VALUE_MSG`      | Host sets a new parameter value                  |
| `2`   | `UPDATE_VALUE_MSG`   | Device sends a live/status value update          |
| `3`   | `RESPONSE`           | Device acknowledges a command or returns a value |

---

## 5. Component IDs

`ComponentID` is an 8-bit parameter namespace. Existing legacy compressor IDs are preserved at the start of the enum. Saturator IDs are assigned explicitly in the `0x80-0x9F` range.

### 5.1 Compressor / legacy control IDs

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

### 5.2 Saturator range: `0x80-0x9F`

| ID     | Name                     | Description                         |
|:------:|--------------------------|-------------------------------------|
| `0x80` | `SAT_GAIN_IN`            | Saturator input gain                |
| `0x81` | `SAT_MIX`                | Dry/wet or processed mix            |
| `0x82` | `SAT_GAIN_OUT`           | Saturator output gain               |
| `0x83` | `SAT_TR_BIAS`            | Transistor stage bias               |
| `0x84` | `SAT_PNT_BIAS`           | Pentode stage bias                  |
| `0x85` | `SAT_TR_PNT_MIX`         | Transistor/pentode blend            |
| `0x86` | `SAT_PREEQ_ON`           | Pre-EQ enable                       |
| `0x87` | `SAT_PREEQ_BAND_1_GAIN`  | Pre-EQ band 1 gain                  |
| `0x88` | `SAT_PREEQ_BAND_2_GAIN`  | Pre-EQ band 2 gain                  |
| `0x89` | `SAT_PREEQ_BAND_3_GAIN`  | Pre-EQ band 3 gain                  |
| `0x8A` | `SAT_POSTEQ_ON`          | Post-EQ enable                      |
| `0x8B` | `SAT_POSTEQ_BAND_1_GAIN` | Post-EQ band 1 gain                 |
| `0x8C` | `SAT_POSTEQ_BAND_2_GAIN` | Post-EQ band 2 gain                 |
| `0x8D` | `SAT_POSTEQ_BAND_3_GAIN` | Post-EQ band 3 gain                 |
| `0x8E` | `SAT_BYPASS`             | Saturator bypass                    |

### 5.3 Host service range

| ID      | Name          | Description          |
|:-------:|---------------|----------------------|
| `250`   | `HOST_LINE_0` | Service / ACK line   |
| `251`   | `HOST_LINE_1` | Service line         |
| `252`   | `HOST_LINE_2` | Service line         |
| `253`   | `HOST_LINE_3` | Service line         |
| `254`   | `HOST_LINE_4` | Service line         |
| `255`   | `HOST_LINE_5` | Service line         |

---

## 6. MessageValue

`MessageValue` is an unsigned 8-bit value. Its interpretation depends on `ComponentID`:

- Continuous parameters use device-specific scaling over `0-255`.
- Boolean parameters usually use `0 = Off`, `1 = On`.
- Discrete selectors use device-specific enumerated values.
- Response packets may use command-result codes such as `GCA_COMMAND_DONE`, `GCA_COMMAND_FAILURE`, and `GCA_COMMAND_UNSUPPORTED`.

---

## 7. CRC4 Calculation

The checksum is a 4-bit CRC over the lower 28 bits of the packet.

```c
uint8_t compute_crc4(uint32_t packet_bits28) {
    return crc4(0, packet_bits28, 28);
}
```

---

## 8. Typical Command Sequences

### 8.1 Read device value

| Packet Field     | Example value                  |
|:-----------------|:-------------------------------|
| ProtocolVersion  | `GCA_LINK_VER_0`               |
| SystemID         | `GCA_NASTY_SATURATOR_ID`       |
| PacketSequence   | `0`                            |
| ComponentID      | `SAT_GAIN_IN`                  |
| MessageID        | `READ_VALUE_MSG`               |
| MessageValue     | `0`                            |
| CheckSum         | CRC4 over lower 28 bits        |

### 8.2 Set saturator parameter

| Packet Field     | Example value                  |
|:-----------------|:-------------------------------|
| ProtocolVersion  | `GCA_LINK_VER_0`               |
| SystemID         | `GCA_NASTY_SATURATOR_ID`       |
| PacketSequence   | `0`                            |
| ComponentID      | `SAT_MIX`                      |
| MessageID        | `SET_VALUE_MSG`                |
| MessageValue     | `128`                          |
| CheckSum         | CRC4 over lower 28 bits        |

### 8.3 ACK response

| Packet Field     | Example value                  |
|:-----------------|:-------------------------------|
| ProtocolVersion  | `GCA_LINK_VER_0`               |
| SystemID         | `PC_HOST_ID`                   |
| PacketSequence   | `0`                            |
| ComponentID      | `HOST_LINE_0`                  |
| MessageID        | `RESPONSE`                     |
| MessageValue     | `GCA_COMMAND_DONE`             |
| CheckSum         | CRC4 over lower 28 bits        |

---

## 9. Extension Guidelines

When adding a new device or feature:

1. Assign a stable `GCA_SYSTEM_ID` value for the device family.
2. Place new `ComponentID` values into an unused or documented range.
3. Keep existing numeric IDs stable for backward compatibility.
4. Document the `MessageValue` scaling for each new control.
5. If a device receives an unsupported `ComponentID`, respond with `GCA_COMMAND_UNSUPPORTED`.

---

## 10. Implementation Notes

- For single-packet commands, use `PacketSequence = 0`.
- For multi-packet transfers, increment `PacketSequence` modulo 32.
- Host software should wait for a `RESPONSE` after `SET_VALUE_MSG` and `READ_VALUE_MSG` unless a specific streaming mode defines otherwise.
- Real-time `UPDATE_VALUE_MSG` packets may be sent continuously and should be buffered or rate-limited by the host application.

---

## License

This protocol specification and reference implementation are released under the **MIT License**.
