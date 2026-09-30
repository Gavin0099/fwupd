# Active Task

## Current Status

- MEM-OBL-00CB5E8FBB19229D / docker-engine: approved prerequisite check reported docker-engine-unavailable; selected Windows container build is blocked.
- Adopted the governance baseline and installed the local hook/Copilot integration.
- Framework pin: `ai-governance-framework` at `24ba84cb0dd374f241b6ba080015aef1d106a022`.
- P40WD-40 Genesys USB Hub investigation: S0 closed, Lenovo CLI code review closed
  (no CLI-level multi-Hub ordering, no CLI-level PUBKEY/RSA/ECDSA handling found),
  evidence archived at `memory/evidence/p40-s0-lenovo-cli-review-20260922.md`.
  S1-C is confirmed from the connected P40WD board: raw string descriptors
  `0x8A` for L10/L20/L30 are 32-byte UTF-16LE values whose 15 low bytes
  exactly match the mapped embedded v0.0.0.39 PROJECT bodies; high bytes are
  zero. Offline UUIDv5 PROJECT IDs are distinct, but live fwupd instance IDs
  remain unobserved. The parser-only S2a slice still exposes embedded bytes;
  its compatibility gate remains disabled pending focused build/test
  validation. `--uss=d` left USB selective suspend disabled, and the pre-run
  setting is unknown.

## Next Steps

- External-repo readiness and governance smoke checks passed.
- Real Copilot lifecycle execution, hardware behavior, and the full container build/test remain unverified.
- S1-D: live fwupd Instance ID/GUID remains unobserved. Do not run
  `fwupdtool get-devices` on this hardware just to collect IDs because Genesys
  setup enters ISP mode. BONDING/PORTNUM/PUBKEY/code-sign-capability remain
  observe-only; raw descriptors and topology are recorded at
  `memory/evidence/p40-s1-runtime-project-comparison-20260923.md`.
- Run the focused Genesys parser build/tests in the documented container before
  deciding whether to enable the S2b compatibility gate. S3-S7 and any
  hardware/firmware write remain blocked.

- P40 S1 partially complete: decoded runtime PROJECT bodies match the mapped 15-byte payload fields and yield distinct offline UUIDv5 IDs; raw descriptor bytes and live fwupd identity remain unverified. <!-- memory_record_projection:active-task-summary:1a6956742bb96f915c27bfc9a457e3fa9c343ef296b2eab64167b1777f32b4fb -->

- P40 S1-C confirmed: all three captured 0x8A descriptors map exactly from UTF-16LE body to embedded 15-byte PROJECT payloads; live fwupd instance IDs remain unobserved. <!-- memory_record_projection:active-task-summary:1eb69a4b9215aca059eb719494c0f18996019b298c0199c65d8f8e6b9fe85bf4 -->

- P40 S2a parser-only slice validated in the official container: Genesys targets build, focused firmware test passes, and all three P40 payloads parse offline; S2b remains disabled pending an explicit decision, and live identity/hardware behavior remain unverified. <!-- memory_record_projection:active-task-summary:d439a678609e2fffa7dd86e28ff24395d37f876847d14ab4d2bf5fa75a2450cc -->

- P40 S2b-1 P40-specific strict compatibility gate is implemented and focused-tested: P40 VID/PID quirks enable exact 15-byte PROJECT plus raw six-byte mask-IC checks, failing closed on missing/invalid identity; generic Genesys remains unchanged. Live fwupd identity/topology, active-bank cross-check, and hardware update behavior remain unverified. <!-- memory_record_projection:active-task-summary:b7750adbb4b6105910d20b57aaa664811d532137f9ca587849e58e823d267561 -->

- P40 S2b-1 strict PROJECT+raw-IC gate is implemented and focused-tested. S3 live fwupd enumeration remains blocked: usbipd-win is absent, the current PowerShell is non-admin, Ubuntu WSL is stopped, and Docker has no USB passthrough. No USB handoff or hardware probe occurred; proceed only after the owner prepares elevated USB passthrough for the authorized one-shot enumeration. <!-- memory_record_projection:active-task-summary:9e467d8817858e06341dc08e39f55e6e01e2bea284ec590594f4bbd30273bda4 -->

- P40 S2b-1 strict PROJECT+raw-IC gate is implemented and focused-tested. usbipd-win 5.3.0 is installed and Ubuntu WSL is active, but current usbipd list exposes no P40 Hub PID; bus 5-5 / 0000:0002 is PnP port 5 on L1, not the target chain. A bhound10 filter warns --force may be needed. No device bind/attach or fwupd enumeration occurred; next verify exact bus mapping in elevated PowerShell without force-binding. <!-- memory_record_projection:active-task-summary:385130f293a540dc160905b9ee955964232c9991532878fbb12a2f0db186455c -->

- P40 S2b-1 strict PROJECT+raw-IC gate is implemented and focused-tested. usbipd-win 5.3.0 is installed and Ubuntu WSL is active, but the supplied usbipd list omits upstream 1D5C:5801 and P40 hubs 17EF:1151/1153/1155; 5-5 / 0000:0002 maps to L1 port 5, not the target chain. A bhound10 filter warning remains. No USB bind/attach or fwupd probe occurred; next use a Linux or passthrough path that exposes the exact target hubs, without force-binding. <!-- memory_record_projection:active-task-summary:b843e5ecf8f6a89740559809945412920adfd0897c7d0f4244c542ae0efecc2d -->

- P40 S2b-1 strict PROJECT+raw-IC gate is implemented and focused-tested. S3 acceptance now requires the full L1->L2->L3 Hub topology and any Genesys HID proxy/parent-child links. usbipd-win cannot share a whole Hub/port (issue #747) and the current bus list exposes no P40 targets; use native Linux or whole USB-controller passthrough. No live enumeration or firmware write occurred. <!-- memory_record_projection:active-task-summary:45b90457545da35d0bcf00a0fdbee65c2b0f7d32a476f2dc58e502d1eab7a7cc -->

- P40 S2b-1 strict PROJECT+raw-IC gate is implemented and focused-tested. S3 acceptance separates OS enumeration/physical USB topology, fwupd Hub identity, and actual HID proxy/child links. Local source confirms standalone fwupdtool get-devices coldplugs and can send ISP_ENTER via Genesys setup; fwupdmgr GetDevices returns daemon state, but startup/hotplug may invoke setup. No live command has run; keep S3 in a topology-preserving Linux environment with mode-transition handling. <!-- memory_record_projection:active-task-summary:cdb35f05e86d4d6af478bf777a97e187638ea7aa5e2b182e90ed1b42b82e21a6 -->

- S3 acceptance requires OS enumeration and physical L1->L2->L3 paths, fwupd identity for each Hub, and a proxy/child link for each of the three owner-confirmed HID update devices. A one-shot fwupd coldplug may enter ISP mode and that side effect is accepted; no firmware write. Windows usbipd currently exposes no P40 Hub target, so live S3 remains pending a topology-preserving Linux path. <!-- memory_record_projection:active-task-summary:888b99928a246e344bbd59a042d536d080ea88166254ac4ff6abb13667e64159 -->

- S3 acceptance requires OS enumeration and physical L1->L2->L3 paths, fwupd identity for each Hub, and proxy/child verification for each of the three owner-confirmed HID update devices. One-shot fwupd coldplug entering ISP mode is accepted; no firmware write. USBIPD currently exposes no P40 Hub bus, so live S3 awaits native Linux or whole-controller passthrough. <!-- memory_record_projection:active-task-summary:2b3e37dc66bfc8231a07036e79ec4f0891840d4394848caf6397842dbd7126ba -->

- P40 S2b-1 strict PROJECT+raw-IC gate is implemented and focused-tested. S3 requires OS enumeration/physical L1-L2-L3 topology, fwupd identity for each Hub, and successful proxy/child association for HID1/HID2/HID3. ISP_ENTER during one-shot coldplug is accepted; capture each Hub mode and VID:PID/instance path before/after plus USB re-enumeration. No firmware write; current USBIPD list exposes no P40 Hub target. <!-- memory_record_projection:active-task-summary:dcce247158fe3c57a15ef99df5335d9c4f9492d034dfbc727cec4973b4e4d099 -->

- S3 native-Linux probe: physical chain PASS; custom fwupdtool recognises 1153/1155 (GL3523-50) but 1151 (GL3525-10) fails ISP_ENTER with Pipe error on request 0x81; no HID proxy links exist yet. No firmware write; S3 full PASS not claimed. <!-- memory_record_projection:active-task-summary:23e0dbd08c6bf69b35b2ce6f8064520afa84609eb298a770bf76ca2c30402f13 -->

- S3: all three HIDs confirmed as GLI HID (L1 f00f, L2 4c41, L3 4c42); with an extra quirk 4c41/4c42 link as children of 1153/1155, while 1151 still fails ISP_ENTER (0x81 Pipe error) and its HID is dropped. ISP_ENTER-via-HID (Probe B) not run; no firmware write; S3 full PASS not claimed. <!-- memory_record_projection:active-task-summary:2b5f9b0a583250eacea91ec608b3bf928df2f5ada8b47d035ac178140244f8d5 -->

- S3 Probe B inconclusive: ISP_ENTER via the f00f HID was acknowledged but status register 5 already read 0 before it, so H1 stays untested; dock recovered by full power cycle and BASELINE_OK. Next: read-only XROM control on 1153 (draft, not run). No firmware write. <!-- memory_record_projection:active-task-summary:84d1940b15dcd4f4e846b7c7202cd2f0cdb992e3f1fafbabc58ece385a00d4dd -->

- S3: XROM and JEDEC flash reads are a valid ISP observable on 1153 (ff before, XROM/c84012 after ISP_ENTER); register 5 is not. Next is a 1151 probe through the f00f HID using that observable (draft not yet written). No firmware write; H1 untested. <!-- memory_record_projection:active-task-summary:dac347d1e1413c73b9a6e898f1fe8371c869d944a700f5a5b5bc065d5e313537 -->

- S3: an earlier read bug of mine voided the B2/B3 read values; the fixed B3 on 1153 shows HID and direct flash reads return XROM and JEDEC c84012 after ISP_ENTER (ff before). Fixed B2 on 1151 via f00f is ready but not run; H1 untested. No firmware write. <!-- memory_record_projection:active-task-summary:1eae2856ae7bd1577e7c219f12f165e010038dfcd207df058c2eeee7002ea06a -->

- S3: 1151 is reachable only through its f00f HID (flash readable with no ISP_ENTER, direct vendor requests all STALL), reproduced in fresh states; 1153 needs ISP_ENTER on both paths. Open item is a plugin lifecycle design decision. No firmware write; S3 full PASS not claimed. <!-- memory_record_projection:active-task-summary:60ed2cb7573bdcdcfe00f3b8dd8ae79471d6073d3ca48ba8a995176ced04f1eb -->

- S3: implemented hid-transport in the genesys plugin (flag only on 1151, flash setup deferred until the HID proxy is bound, HID quirks for f00f/4c41/4c42); container build and genesys-self-test 6/6 pass; not yet run on the dock. No firmware write. <!-- memory_record_projection:active-task-summary:8dace222bf6456964e2e3be9c5f9117aa67de264f98c74f1612846e7db44e59b -->

- S3: hid-transport validated on the dock for setup (1151 listed, f00f child, CFI c8 40 12 via HID; 1153/1155 unchanged). Pending: dock power cycle and baseline, clang-format, emulation tests, commit decision. No firmware write. <!-- memory_record_projection:active-task-summary:05645eefb94f7af072995095cbf48164e711a569defc6493f1536b526fbd34bd -->

- S4 started offline: composite cabinet drafts build; fwupd defines no order among the three hubs; payload versions appear older than installed. Blocked on owner decisions on target version, hub order and ParentGuid. No hardware write. <!-- memory_record_projection:active-task-summary:3022b0f03ce932c77e2f93ad41e6379c9882e7a403babcdfcbc0cae9f7d75556 -->

- S4 static work done: payloads older than installed, vendor order L1 then L2 then L3 with reset last, fwupd resets per hub so order and reset policy need a design decision. Blocked on target version and order policy. No hardware write. <!-- memory_record_projection:active-task-summary:9d29193c8824366af5239e3d007f8c0c4eedcc343b29a24ec97240a393c259e0 -->

- S5 preparation done statically: write path is erase-first on bank1 with no rollback; per-hub installs in order L3, L2, L1 need no code change. No write started; awaiting authorisation, backup, payload source and recovery plan. <!-- memory_record_projection:active-task-summary:dfdb2463350053d84b8912287cd7e9dcd85ff8cf601e37b0c5eba81b1d9ecf91 -->

- Write not started: the offered P34WD-40 firmware has different PROJECT strings and versions from the installed P40 hubs. Need a P40 same-version payload source. <!-- memory_record_projection:active-task-summary:0532e7349377c2fc3a6ae0a586aa02f3c89a989ab0a6082785c1620deceaa2ab -->

- L3 backup taken and same-version 82.7 image and cabinet prepared offline; no write started; awaiting dock power cycle, baseline and an explicit go on the new facts; L2 and L1 not dumped. <!-- memory_record_projection:active-task-summary:4171e52f9ac181a4cdbf81e8bcdcf1746df0361b6b6855c44be651df9bfef9e2 -->

- Codesign question answered statically: P40 hubs are non-codesign in both the CLI and fwupd; ISP_EXIT versus attach reset difference is open; no write started; awaiting the owner choice among L3 write, dual-image evaluation or L2 and L1 dumps. <!-- memory_record_projection:active-task-summary:1b874ea07bf7847f803c067a2bf6f1186395cf06fb748c1383bda685e411f048 -->

- Lenovo end-to-end flow reviewed and compared with fwupd; open before a write: ISP exit versus reset and the bank policy; no write started; awaiting owner choice. <!-- memory_record_projection:active-task-summary:3b72e9ceaf81b705e99ea0ac0fa6441e1d9f9fca820afb006c353163059b091e -->

- S4.1 done offline: exit-isp-before-reset flag on the three P40 hubs, attach sends ISP exit then reset; tests pass, tarball prepared, not committed, no hardware touched. Next S4.2 bank policy and S4.3 dumps; no write without an explicit go. <!-- memory_record_projection:active-task-summary:a69cba6ace0f7d469b31b5f42ef4d698c45b307ad3d614e36bf4f3d4c27b2067 -->

- S4.2 done: dual-image adds nothing for a same-version write, stays off; S4.3 dumps with the exitisp build are next and need the NUC; no write without an explicit go. <!-- memory_record_projection:active-task-summary:4562ee22ae04182f6e1e7abcbce288a9042be56a83e9a2c7c91016d89584f4d4 -->

- HP ISP Flow notes checked: not applicable to the non-codesign P40 hubs; S4.3 dumps with the exitisp build wait for the dock power cycle and baseline; no write. <!-- memory_record_projection:active-task-summary:a4cc9be58ed796a5981fbe8aa269a8a8c152377338984bae8c7807545bcf2174 -->

- L3 dump with the exitisp build done: ISP exit then reset seen on hardware, dump identical to the first; next baseline, power cycle, then L2 and L1 dumps; no write without an explicit go. <!-- memory_record_projection:active-task-summary:f99ca463b302c264562790d96e2f54f7810e4c3e1df9dae2e4fdc291ed40e64b -->

- MEM-OBL-5E77875AE8C1ADE1 / docker-engine: approved prerequisite check reported docker-engine-unavailable (exit 1); selected Windows container build is blocked; controlled Docker response unchanged, no build run. <!-- memory_record_projection:active-task-summary:864ab9b2724e7660a950660757f4047a56364822367f5b47916bb89b148a7334 -->
