# Candidate for the explicitly selected Windows Docker-container build workflow.
# PowerShell 7 is required, matching the obligation handler's pwsh recheck.
[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
# This judgment parameter is part of the approved checker bytes, not caller input.
$dockerReadinessDeadlineMilliseconds = 5000

function Write-BuildBlocker([string] $Reason) {
    Write-Output 'GOVERNANCE_EVENT_V1 {"id":"FWUPD-BUILD-DOCKER-001","type":"required_dependency_blocked","dependency":"docker-engine","status":"blocked"}'
    Write-Output "Windows Docker-container build blocked: $Reason"
}

$query = $null
$outcome = 'error'
$reason = 'docker-query-error'
try {
    $docker = Get-Command docker.exe -CommandType Application -ErrorAction SilentlyContinue | Select-Object -First 1
    if (-not $docker) {
        $outcome = 'blocked'
        $reason = 'docker-cli-missing'
    }
    else {
        $start = [System.Diagnostics.ProcessStartInfo]::new()
        $start.FileName = $docker.Source
        $start.UseShellExecute = $false
        $start.CreateNoWindow = $true
        $start.RedirectStandardOutput = $true
        $start.RedirectStandardError = $true
        $start.ArgumentList.Add('info')
        $start.ArgumentList.Add('--format')
        $start.ArgumentList.Add('{{.ServerVersion}}')
        $query = [System.Diagnostics.Process]::new()
        $query.StartInfo = $start
        if (-not $query.Start()) { throw 'process-start-failed' }
        # Drain both streams asynchronously; stderr content is never published.
        $stdout = $query.StandardOutput.ReadToEndAsync()
        $stderr = $query.StandardError.ReadToEndAsync()
        if (-not $query.WaitForExit($dockerReadinessDeadlineMilliseconds)) {
            $query.Kill($true)
            if (-not $query.WaitForExit(1000)) { throw 'process-cleanup-failed' }
            $outcome = 'blocked'
            $reason = 'docker-query-timeout'
        }
        elseif ($query.ExitCode -ne 0) {
            $outcome = 'blocked'
            $reason = 'docker-engine-unavailable'
        }
        else {
            if (-not [System.Threading.Tasks.Task]::WaitAll([System.Threading.Tasks.Task[]]@($stdout, $stderr), 1000)) {
                throw 'process-streams-incomplete'
            }
            $version = $stdout.GetAwaiter().GetResult().Trim()
            $null = $stderr.GetAwaiter().GetResult()
            if ($version -cmatch '^[0-9]+\.[0-9]+(?:\.[0-9]+)?(?:[-+][A-Za-z0-9.-]+)?$') {
                $outcome = 'ready'
            }
            else {
                $reason = 'docker-version-response-invalid'
            }
        }
    }
}
catch {
    # Preserve unknown as an error; do not turn unexpected failures into events.
    $outcome = 'error'
    $reason = 'docker-query-error'
}
finally {
    if ($query) { $query.Dispose() }
}

if ($outcome -eq 'blocked') {
    Write-BuildBlocker $reason
    exit 42
}
if ($outcome -eq 'ready') {
    Write-Output 'Windows Docker-container build prerequisite ready: Docker Engine responded.'
    exit 0
}
[Console]::Error.WriteLine("Windows Docker-container prerequisite check error: $reason")
exit 2
