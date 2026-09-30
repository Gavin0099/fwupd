# Review Log

## Entries

- Append review summaries and validation history here.
- 2026-09-22: Closed the read-only Lenovo CLI code review (`Source/Lenovo`, 22
  files + 9 tests) for the P40WD-40 Genesys USB Hub investigation. No CLI-level
  multi-Hub ordering and no CLI-level PUBKEY/RSA/ECDSA handling found. Findings
  and explicit non-claims archived at
  `memory/evidence/p40-s0-lenovo-cli-review-20260922.md`. No fwupd code changed.
- 2026-09-22: Scoped the pre-hardware S2a Genesys work to parser-only WIP on the
  P40 feature branch: retain embedded raw BrandProject bytes for later S1
  comparison. Removed the `check_firmware` exact-byte rejection gate because
  runtime and firmware length/padding/termination semantics are not proven.
  Direct P40 evidence covers GL3523 `0x281` and GL3525 v2 `0x331`; other
  format mappings are not claimed as P40 package proof. Focused build/test
  remains pending because the Windows host lacks Meson/C compiler and Docker
  daemon access.
