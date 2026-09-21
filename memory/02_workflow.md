# Tech Stack

## Repo Facts

- Runtime: Linux firmware update daemon and command-line tools; development automation runs in the documented privileged fwupd container.
- Primary languages: C with GLib; Meson build definitions; Python and shell tooling; Rust components under `rust/`.
- Build: `build-fwupd` from the project virtual environment, after dependency setup with `contrib/ci/fwupd_setup_helpers.py`.
- Tests: `test-fwupd` from the project virtual environment; focused plugin checks use `fwupdtool --plugins PLUGIN ...`.
- Governance validation: `ai-governance-framework/governance_tools/governance_drift_checker.py`, `external_repo_readiness.py`, and `external_repo_smoke.py`.
- High-risk boundary: firmware parsing/install flows, device state, trust/signature checks, plugin infrastructure, public D-Bus/API/ABI surfaces, and release packaging.
