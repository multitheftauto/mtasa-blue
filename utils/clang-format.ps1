#!/usr/bin/env pwsh

# cd to parent location
$repoRoot = Split-Path -Parent $PSScriptRoot
Push-Location $repoRoot

try {
    # download clang-format if needed
    if ($isLinux) {
        $clangFormatUrl = "https://github.com/multitheftauto/llvm-project/releases/download/llvmorg-21.1.8/clang-format-armv7a-linux-gnueabihf"
        $clangFormatFilename = "clang-format"
        # TODO: Update this hash with the actual Linux binary hash
        $expectedHash = "LINUX_HASH_PLACEHOLDER"
    } else {
        $clangFormatUrl = "https://github.com/multitheftauto/llvm-project/releases/download/llvmorg-21.1.8/clang-format.exe"
        $clangFormatFilename = "clang-format.exe"
        $expectedHash = "b77927335fd4caece863ea0c09ec8c4dfbdab545510c9091205ccfb528d5abf2"
    }

    $binDir = Join-Path $repoRoot "Build" "tmp"
    $clangFormatPath = Join-Path $binDir $clangFormatFilename

    $shouldDownload = $true
    if (Test-Path $clangFormatPath) {
        $currentHash = (Get-FileHash -Path $clangFormatPath -Algorithm SHA256).Hash.ToLower()
        if ($currentHash -eq $expectedHash) {
            $shouldDownload = $false
        } else {
            Write-Host "clang-format.exe hash mismatch, re-downloading"
        }
    }

    if ($shouldDownload) {
        Write-Host "Downloading clang-format..."
        if (-not (Test-Path $binDir)) {
            New-Item -ItemType Directory -Path $binDir | Out-Null
        }
        Invoke-WebRequest -Uri $clangFormatUrl -OutFile $clangFormatPath
        $downloadedHash = (Get-FileHash -Path $clangFormatPath -Algorithm SHA256).Hash.ToLower()
        if ($downloadedHash -ne $expectedHash) {
            Write-Error "SHA256 hash mismatch! Expected: $expectedHash, Got: $downloadedHash"
            exit 1
        }

        # Make executable on Linux
        if ($isLinux) {
            chmod +x $clangFormatPath
        }

        Write-Host "clang-format downloaded successfully"
    }

    # compute list of files to format
    $searchFolders = "Client", "Server", "Shared"
    $files = Get-ChildItem -Path $searchFolders -Include *.c, *.cc, *.cpp, *.h, *.hh, *.hpp -Recurse | Select-Object -ExpandProperty FullName

    # save files to a temp file
    $tmp = [System.IO.Path]::GetTempFileName()
    $files | Out-File $tmp -Encoding utf8

    # clang-format in place, clean up temp file
    & $clangFormatPath -i --files=$tmp
    Remove-Item $tmp
} finally {
    Pop-Location
}
