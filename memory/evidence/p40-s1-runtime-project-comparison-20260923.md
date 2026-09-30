# P40WD-40 S1 Runtime PROJECT Comparison

Date: 2026-09-23
Status: partial S1 evidence; no firmware write or live fwupd setup was run.

## Runtime Evidence

Source log: `E:\BackUp\Git_EE\cli\winbuild\Source\Release\Log\Test_Log_GLUpdateTool_0923_161955.txt`.
The log was produced by `GLUpdateTool.exe info -hu --uss=d` at 2026-09-23 16:19:55 local time. It reports four candidate USB hubs; three are accepted as Genesys devices and classified as GL3525, GL3523, and GL3523. Their decoded PROJECT strings are `LNV_P40WD40_L10`, `LNV_P40WD40_L20`, and `LNV_P40WD40_L30`.

The PnP tree observed after the command was:

`USB root port 8 -> USB 1D5C:5801 -> 17EF:1151 port 4 -> 17EF:1153 port 1 -> 17EF:1155 port 2`

The USB hub `bcdDevice` descriptor values are `8009`, `8106`, and `8207` for PIDs `1151`, `1153`, and `1155`; these are USB descriptor revision values, not IC identifications. The CLI classified the corresponding devices as GL3525, GL3523, and GL3523.

## Raw Runtime Descriptor Capture

Each device's standard USB string descriptor index `0x8A` was captured once with `IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION`, using the current parent-hub connection index. The request used `wValue=0x038A` and `wLength=64`; the returned setup packet identifies a device-to-host `GET_DESCRIPTOR` request (`bmRequestType=0x80`, `bRequest=0x06`). The parent hub handle was opened with the same `GENERIC_READ | GENERIC_WRITE` access mask used by the CLI helper; the only transfer sent was this standard IN descriptor request. `BytesReturned=44` for each request: the 12-byte request structure plus the 32-byte descriptor.

| Hub | Parent hub / connection index | Descriptor header | Complete descriptor bytes |
|---|---|---|---|
| L10 | `1D5C:5801` / 4 | `20-03` | `20-03-4C-00-4E-00-56-00-5F-00-50-00-34-00-30-00-57-00-44-00-34-00-30-00-5F-00-4C-00-31-00-30-00` |
| L20 | `17EF:1151` / 1 | `20-03` | `20-03-4C-00-4E-00-56-00-5F-00-50-00-34-00-30-00-57-00-44-00-34-00-30-00-5F-00-4C-00-32-00-30-00` |
| L30 | `17EF:1153` / 2 | `20-03` | `20-03-4C-00-4E-00-56-00-5F-00-50-00-34-00-30-00-57-00-44-00-34-00-30-00-5F-00-4C-00-33-00-30-00` |

All three descriptors have `bLength=0x20` and `bDescriptorType=0x03`. Their 30-byte bodies contain 15 UTF-16LE ASCII code units: every even-position body byte is the corresponding PROJECT ASCII byte and every high byte is `0x00`. No terminator/padding occurs inside the declared descriptor; the remaining bytes in the 64-byte request buffer are zero-filled outside `bLength`.

## Embedded Payload Comparison

The P40 v0.0.0.39 package is located at:

`C:\Users\reiko\Desktop\Backup\0317\Lenovo_ISP_Tool_P40WD-40_v0_0_0_39\Firmware\Hub\HubFW`

Each payload was read locally with `File.ReadAllBytes`; SHA-256 was calculated from the same file. The fwupd parser constants select the GL3525 v2 PROJECT offset `0x331` when configuration byte `0x100` is `0xA5` or `0xA6`, and GL3523 offset `0x281`. The PROJECT body length is 15 bytes.

| Hub | Runtime USB VID:PID / bcdDevice | CLI IC / PROJECT | Payload | Offset | Embedded 15-byte hex | SHA-256 | Body match |
|---|---|---|---|---|---|---|---|
| L10 | `17EF:1151` / `8009` | GL3525 / `LNV_P40WD40_L10` | `GL3525-OVY1L_LNV_P40WD_L1_FW8000.bin` (45056 bytes, config `0xA5`) | `0x331` | `4C-4E-56-5F-50-34-30-57-44-34-30-5F-4C-31-30` | `24BE766320267DA0656B24C9E7D444FDC840CD53BD5922A98C250E7FCD697146` | exact 15-byte body match |
| L20 | `17EF:1153` / `8106` | GL3523 / `LNV_P40WD40_L20` | `GL3523-OTY50__LNV_P40WD-40_L2_FW8100.bin` (31744 bytes) | `0x281` | `4C-4E-56-5F-50-34-30-57-44-34-30-5F-4C-32-30` | `5F3F034D2CC5682395E5DD97B4EBEF89AA444777ADE71A9313EE98F7CD503A33` | exact 15-byte body match |
| L30 | `17EF:1155` / `8207` | GL3523 / `LNV_P40WD40_L30` | `GL3523-OTY50__LNV_P40WD-40_L3_FW8203.bin` (31744 bytes) | `0x281` | `4C-4E-56-5F-50-34-30-57-44-34-30-5F-4C-33-30` | `78331639CC86E8CB8DD91618471584531C7ABA93D653BB043D1F404DFF5800C8` | exact 15-byte body match |

For each row, the extracted payload bytes equal the low byte of every UTF-16LE code unit in the captured runtime descriptor body. Thus the current three samples establish the exact runtime descriptor-to-firmware 15-byte body mapping, including descriptor header, declared length, and UTF-16LE high-byte behavior.

## Expected fwupd Identity

Source inspection shows fwupd hashes the parsed 15-byte PROJECT data using `fwupd_guid_hash_data(..., FWUPD_GUID_FLAG_NONE)`, uppercases that GUID as the `PROJECT` instance value, and builds `USB\VID_...&PID_...&PROJECT_...`.

Offline UUIDv5 results from the body bytes above are:

| Hub | Calculated PROJECT GUID | Expected instance ID |
|---|---|---|
| L10 | `332EF9A7-81A0-5CBF-9675-8BAA1D062D4A` | `USB\VID_17EF&PID_1151&PROJECT_332EF9A7-81A0-5CBF-9675-8BAA1D062D4A` |
| L20 | `1301CA55-0010-5DE6-A0F2-9F6C3E76CBB7` | `USB\VID_17EF&PID_1153&PROJECT_1301CA55-0010-5DE6-A0F2-9F6C3E76CBB7` |
| L30 | `950B1155-065C-5AEE-A72D-B976CD09F169` | `USB\VID_17EF&PID_1155&PROJECT_950B1155-065C-5AEE-A72D-B976CD09F169` |

The offline UUIDv5 calculation was checked against the repository's `python.org` known-answer vector: expected and calculated GUID were both `886313e1-3b8a-5372-9b90-0c9aee199e5d`. These are calculated expected IDs, not observed IDs from a live fwupd device instance.

## Claim Boundary

- Supported: the three captured runtime descriptors are length `0x20`, type `0x03`, and decode to distinct PROJECT strings; each descriptor body maps exactly to the 15-byte PROJECT body embedded in its mapped v0.0.0.39 payload. The current fwupd source would derive three distinct PROJECT UUIDs and expected USB instance IDs from those bodies.
- Capture scope: this representation contract is directly evidenced for these three present descriptors. It does not qualify other firmware revisions or future hardware states.
- Not observed: GUIDs/instance IDs produced by a live fwupd enumeration. `fwupdtool get-devices` was not run because Genesys device setup enters ISP mode.
- Not claimed: update ordering, firmware-write behavior, or hardware-update correctness.
- USB selective suspend is currently disabled; the CLI log reports this was disabled by `--uss=d`. The pre-run setting was not recorded, so no restoration was made.
