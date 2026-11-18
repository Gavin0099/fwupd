# PLAN.md
<!-- governance-baseline: overridable -->
<!-- baseline_version: 1.0.0 -->

> **最後更新**: 2026-11-18
> **Owner**: fwupd maintainers
> **Freshness**: Sprint (7d)

---

## Current Phase

<!-- Required: fill in current phase ID and description -->

- [ ] Phase A : AI Governance onboarding and fork CI repair

## Active Sprint

<!-- Required: list current sprint tasks -->

- [x] Add the canonical `ai-governance-framework` submodule and pin its adopted commit.
- [x] Generate the governance baseline, rule pack, memory scaffold, and drift workflow.
- [x] Install and validate the local governance hooks and Copilot managed surface.
- [x] Run onboarding readiness and governance runtime smoke checks; record the evidence and remaining limits.
- [x] Update the framework pin and run the drift checker from the pinned submodule.
- [ ] Confirm the Governance Drift check on the fork PR and verify that a fresh Copilot session loads the fork's rules.

## Backlog

<!-- Required: prioritized items not yet started -->

- P1: Keep the framework submodule and `governance/framework.lock.json` synchronized through the governed update path.
- P1: Add fwupd-specific validators only when a concrete domain risk and executable evidence path justify them.

## Decision Log

<!-- Optional but recommended: record architecture or governance decisions with dates -->

<!-- Example:
- 2026-03-21: Chose X over Y because Z
-->

- 2026-09-21: Adopted the canonical framework as a git submodule so the framework source and parent-repository pin are reviewable together.
- 2026-09-21: Preserved the existing Copilot instructions and merged only the framework-managed block through the official installer.
- 2026-11-18: Keep governance on the fork's own branch; base upstream PR branches on upstream `main` and do not merge the fork's governance changes into them.

## Known Risks

<!-- Optional: track identified risks and mitigation status -->

- Runtime lifecycle execution in a real Copilot session and hardware behavior are not proven by the onboarding smoke; keep those claims bounded.
- Full fwupd build and test validation requires the documented privileged container and may be unavailable on this Windows host.
