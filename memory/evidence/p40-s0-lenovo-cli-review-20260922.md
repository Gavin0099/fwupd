# P40WD-40 — Lenovo CLI Review Evidence Archive

Status: read-only evidence archive. No fwupd code was changed as part of the
CLI review. No hardware was accessed. Later P40 parser work is tracked
separately and does not change the conclusions in this archive. This file exists
to give the `cli-20260922-100641` canonical memory
record (in [2026-09-22.md](../2026-09-22.md)) a durable, repo-tracked evidence
target, resolving the `test_evidence_provenance_not_found` warning raised
against that record by `governance_tools.memory_authority_guard`.

## Scope

- `Source/Lenovo/CliArgParser.cpp`, `Source/Lenovo/CliArgParser.h`, `Source/Lenovo/Main.cpp`
- `Source/Lenovo/Library/` — 22 files (CliArgParserLib, CliCheck, CliDefineLenovo,
  CliGetScalerInfo, CliHelpEx, CliIniSetting, CliIspmatch, CliIspmp, CliRestore,
  CliSetScalerInfo, CliUniUpdate, CliUpdateCondition, plus their `.h` headers)
- `TestSuite/UTest/Lenovo/` — 9 test files
- Cross-reference only (signatures, not a full re-analysis — internals already
  covered by the S0 mapping record): `SubModule/IspEngine_Lib/Source/Entry/libMain.h`

Method: Explore subagent (thoroughness=thorough) over the full scope above,
spot-verified directly via `read_file` at:

- `Source/Basic/CliBase.cpp:55-70`
- `Source/Lenovo/Library/CliCheck.cpp:40-80`
- `SubModule/IspEngine_Lib/Source/Entry/libMain.h:120-220`

## Verified conclusions (code-level, read-only)

### Command → API map

| Command | Core API | Location |
|---|---|---|
| `check` | `CheckFW_lenovo_Hub_byMatchIni`, `GetOciFwVersion`, `GetOciBinFileInfo` | `CliCheck.cpp:47,62,63` |
| `getscalerinfo` | `GetLadmHardwareType/ModelName/OsdId/SerialNumber/FwVersion` | `CliGetScalerInfo.cpp:67,96,103,110,117,124` |
| `ispmatch` | `UpdateFW_lenovo_Hub_byMatchIni` | `CliIspmatch.cpp:56,58,60` |
| `ispmp` | `UpdateFW_lenovo_monitor_byModelPanel` | `CliIspmp.cpp:57,64,66` |
| `restore` | `GetHubNum`, external `usbhidtool.exe` | `CliRestore.cpp:52,54,65,90,98` |
| `setscalerinfo` | `SetLadmOsdId` | `CliSetScalerInfo.cpp:60,66,84` |
| `updatecondition` | `GetDeviceList`, `IsFwAvailable`, `SendUsbSetupPacket` (self-built Realtek/MStar packets) | `CliUpdateCondition.cpp:67,71,80,101,142,216` |
| `uniupdate` | same primitives, opcode `0xce`, sub-commands `A0/A3/A5/A6` | `CliUniUpdate.cpp:27,89,130,206,210,221,271,321` |

1. Dispatch: `Source/Lenovo/Library/CliArgParserLib.cpp:198` — Lenovo factory
   registers 9 commands (`help`, `check`, `getscalerinfo`, `ispmatch`, `ispmp`,
   `restore`, `setscalerinfo`, `updatecondition`, `uniupdate`), falls back to
   shared `CreateCli` otherwise.
2. No Lenovo-only fixed model / VID-PID matching found (no `MDGG`, `334wp`, or
   hard-coded VID/PID table) anywhere in the reviewed Lenovo-layer files.
3. No CLI-level multi-Hub ordering/sequencing code found. Each command operates
   on exactly one `_iDeviceId` per invocation (`CliIspmatch.cpp`, `CliCheck.cpp`,
   `CliUpdateCondition.cpp:71`, `CliUniUpdate.cpp:210`). `GetDeviceList` is only
   used to index into a flat device list, never to traverse parent/child/port
   topology.
4. No CLI-level `PUBKEY`/RSA/ECDSA handling found. The only code-sign-adjacent
   branch in this layer is the existing shared dispatch to `ManualUpdateSignedFW`
   when the runtime device reports `HasCodesign()` (already recorded in the S0
   mapping entry, `cli-20260922-095853`); the CLI layer adds nothing beyond that
   dispatch.
5. Windows driver/USS handling (`-dri=s`/`-dri=p`, `DetermineHubDriverDecision`,
   `CCliIndFunction`) is shared `CCliBase` infrastructure, not a Lenovo-specific
   update algorithm (`CliBase.cpp:55-119`, used by `CliIspmatch.cpp:45`,
   `CliIspmp.cpp:45`, `CliRestore.cpp:45`).
6. Lenovo-specific CLI plumbing found and judged out of scope for P40 Hub
   firmware logic: `/msgwnd`, `/msgport`, `/logpath` argument stripping in
   `ProcessAndFilterSpecArg` (`CliArgParserLib.cpp:40,112,126,145,168`), and the
   Realtek/MStar raw scaler USB/I2C packet construction in `updatecondition` /
   `uniupdate` (scaler-specific, not Hub-firmware-specific).
7. Test coverage is shallow: all 9 `TestSuite/UTest/Lenovo/*` files are
   parser/factory-argument tests. None validate actual firmware selection,
   Hub update results, USB packet write effects, or restore success.

## Explicit non-claims

- **P40 offset scope:** direct static inspection of P40 v0.0.0.39 proves only
   GL3523 L20/L30 PROJECT bytes at `0x281` and GL3525 v2 L10 PROJECT bytes at
   `0x331`. The other Genesys offsets used by the parser are existing format
   mappings from the Genesys code/format surface; this archive does not claim
   that the three P40 binaries directly prove GL3523PLUS, GL3590, or GL3525
   legacy offsets.

- **Hub independence is NOT proven.** Absence of sequencing code in the CLI
  layer is evidence of absence at that layer only.
- **Hub ordering is NOT disproven.** *(Correction to an earlier, overstated
  internal note: "if ordering exists it must live inside IspEngine_Lib" claimed
  more certainty than the evidence supports.)* The precise statement is: the
  CLI layer has no sequencing logic; if the Windows update flow has sequencing,
  it may exist in `IspEngine_Lib`, in upper-layer package orchestration (driven
  by `updater_setting_list.json`), or emerge purely from USB topology /
  re-enumeration behavior (e.g. writing L10 resets the hub and L20/L30
  transiently disappear) with no explicit sequencing code anywhere. This is why
  the S1 matrix keeps a `parent/topology` field as observe-only.
- **The company code-sign layer is NOT identified.** Absence of Lenovo-CLI-level
  RSA/ECDSA/PUBKEY handling narrows where it could live; it does not identify
  which layer (`IspEngine_Lib`, hub hardware, or Windows tool orchestration)
  actually performs it for the P40 package, nor whether P40 uses it at all (see
  the `2026-09-21` S0 record: P40 v0.0.0.39 payload evidence shows
  `uHpProprietary=0`, no RSA `N=`/`E=` text found).
- **Runtime device identity has NOT been captured.** No VID/PID, IC/revision,
  PROJECT raw bytes, GUID, BONDING, PORTNUM, PUBKEY, or topology/parent data has
  been read from real hardware in this or any prior slice.
- This CLI review archive does not provide build validation, runtime device
   identity, firmware writes, or evidence for the later parser-only S2a work.
