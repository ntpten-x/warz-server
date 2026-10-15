# Import WarZ + BreezeNet schemas into LocalDB for local private server
$ErrorActionPreference = "Stop"
$server = "(localdb)\MSSQLLocalDB"
$root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

function Exec-Batches([string]$connStr, [string]$sqlFile) {
  Write-Host "Importing $sqlFile ..."
  $text = [IO.File]::ReadAllText($sqlFile)
  $batches = [regex]::Split($text, '(?m)^\s*GO\s*$')
  $conn = New-Object System.Data.SqlClient.SqlConnection $connStr
  $conn.Open()
  $i = 0
  foreach ($batch in $batches) {
    $b = $batch.Trim()
    if ($b.Length -eq 0) { continue }
    $i++
    $cmd = $conn.CreateCommand()
    $cmd.CommandTimeout = 120
    $cmd.CommandText = $b
    try {
      [void]$cmd.ExecuteNonQuery()
    } catch {
      # Ignore DROP of missing objects; fail on real errors later
      $msg = $_.Exception.Message
      if ($msg -notmatch 'Cannot drop|does not exist|There is already') {
        Write-Warning "Batch $i : $msg"
      }
    }
  }
  $conn.Close()
  Write-Host "Done $sqlFile ($i batches)"
}

$master = "Server=$server;Integrated Security=True;TrustServerCertificate=True"
$connMaster = New-Object System.Data.SqlClient.SqlConnection $master
$connMaster.Open()
foreach ($db in @("WarZ", "BreezeNet")) {
  $cmd = $connMaster.CreateCommand()
  $cmd.CommandText = @"
IF DB_ID(N'$db') IS NULL CREATE DATABASE [$db];
"@
  [void]$cmd.ExecuteNonQuery()
  Write-Host "Database $db ready"
}
$connMaster.Close()

Exec-Batches "Server=$server;Database=WarZ;Integrated Security=True;TrustServerCertificate=True" "$root\WarZ-dbo.sql"
Exec-Batches "Server=$server;Database=BreezeNet;Integrated Security=True;TrustServerCertificate=True" "$root\BreezeNet-dbo.sql"

Write-Host "LocalDB import finished."
