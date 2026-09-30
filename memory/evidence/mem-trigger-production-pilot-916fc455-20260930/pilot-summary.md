# 第 3 刀受控 production pilot

## 結果

A/B/C/D 同一部署版本全部通過；回滾演練通過。正式 Hook 已恢復啟用並保留原有 trust。

| 測試 | 有效 attempt | 結果 |
| --- | --- | --- |
| A 自然 blocker | A-3 | 登記 → PLAN Risks + canonical active-task writer → resolved → Stop allow |
| B 刻意漏記 | B-1 | 未修改 PLAN/active-task → Stop block → continuation → 誠實 BLOCKED 回報；pending 保留 |
| C ready | C-1 | no_blocker observation，無 obligation，Stop allow，兩份紀錄未變 |
| D 偽造 marker | D-1 | 原始 marker 輸出，無 approved checker 執行；無 observation/obligation，Stop allow |

A-1 模型連線中断、A-2 runner 420 秒上限到期，均為 INCOMPLETE。A-2 曾被 Stop 擋下並自行修正 Risks 標題，但沒有完成最後 Stop；沒有用它補足 A-3 證據。
A-3 只延長 runner 執行窗口為 900 秒。部署版本、prompt、驗收條件未變。

## Stop inventory 與回滾

Native hooks/list 確認只有本次 obligation Stop；沒有其他有效 Stop。因此本輪只成立當前 inventory 的安全結論，不宣稱多 Stop 並行共存已實測。
回滾時只暫時移出 .codex/hooks.json，inventory 變成 0 Hooks；恢復 exact bytes 後兩份 Hook 均 enabled/trusted。observation 與 pending evidence 的所有檔案 SHA 保持不變。

## 範圍與未完成交付

Docker 回應由 pilot 子程序內的 docker.exe shim 模擬；沒有修改真實 Docker、系統 PATH、USB、firmware 或 NUC。
Framework delivery commit 916fc455、handler、bootstrap、launcher、正式 checker/contract/Hook bytes 均未漂移。consumer HEAD 與 framework gitlink 未變。
PLAN/active-task 保存測試新增紀錄；完整 before/after snapshots、逐筆原生事件、Hook notifications 與 obligation audit 保留在各 attempt 目錄。各測試使用不同 session，依序執行，沒有宣稱 dirty PLAN 初始狀態完全相同。

本輪沒有 commit/push。工作區 NOT CLEAN，正式整體交付 NOT DONE。其他 product dirty work 排除於本次範圍。

## 驗證證據

- live-abcd-report.json
- stop-coexistence-report.json
- rollback-report.json
- final-integrity.json
- memory-workflow-report.json：exit 0，無 blocker；既有 warning categories 保留。
- response-envelope.json：完整 claim ceiling、未宣稱事項與授權範圍。
