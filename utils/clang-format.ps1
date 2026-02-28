#!/usr/bin/env pwsh

function Get-ClangFormat {
    param(
        [Parameter(Mandatory)] [string]$RepoRoot,
        [string]$Version = "v21.1.7"
    )

    if ($isLinux) {
        $clangFormatUrl = "https://github.com/multitheftauto/clang-tools-static-binaries/releases/download/${Version}/clang-format-21_linux-amd64"
        $clangFormatFilename = "clang-format"
        $expectedHash = "3d75779cdc69b06f7e39946b0e50d6ce3dccda1d766e355bf4cf507b1ae13303"
    } else {
        $clangFormatUrl = "https://github.com/multitheftauto/clang-tools-static-binaries/releases/download/${Version}/clang-format-21_windows-amd64.exe"
        $clangFormatFilename = "clang-format.exe"
        $expectedHash = "11defe615493876745f36fb77e9783e7eed03a7f800044ff58619d9293e79409"
    }

    $binDir = Join-Path $RepoRoot "Build" "tmp"
    $clangFormatPath = Join-Path $binDir $clangFormatFilename

    # Check existing file
    if (Test-Path $clangFormatPath) {
        $currentHash = (Get-FileHash -Path $clangFormatPath -Algorithm SHA256).Hash
        if ($currentHash -eq $expectedHash) {
            return $clangFormatPath
        }
        Write-Warning "clang-format hash mismatch, re-downloading..."
    }

    # Download process
    Write-Host "Downloading clang-format..." -ForegroundColor Cyan
    if (-not (Test-Path $binDir)) {
        New-Item -ItemType Directory -Path $binDir | Out-Null
    }

    Invoke-WebRequest -Uri $clangFormatUrl -OutFile $clangFormatPath
    if ($isLinux) {
        chmod +x $clangFormatPath
    }
    Write-Verbose "Downloaded clang-format to $clangFormatPath"

    # Verify hash
    $downloadedHash = (Get-FileHash -Path $clangFormatPath -Algorithm SHA256).Hash
    if ($downloadedHash -ne $expectedHash.ToUpper()) {
        Remove-Item $clangFormatPath -ErrorAction SilentlyContinue
        Write-Error "SHA256 hash mismatch! Expected: $expectedHash, Got: $downloadedHash. The download may be corrupted."
        exit 1
    }

    Write-Verbose "clang-format verified successfully."
    return $clangFormatPath
}

function Invoke-ClangFormat {
    [CmdletBinding()]
    param()

    $arguments = @('-i')
    if ($VerbosePreference -eq 'Continue') {
        $arguments += '--verbose'
    }

    $repoRoot = Split-Path -Parent $PSScriptRoot
    Push-Location $repoRoot
    Write-Verbose "Changed directory to repository root: $repoRoot"

    try {
        Write-Verbose "Searching for source files to format..."
        $clangFormatPath = Get-ClangFormat -RepoRoot $repoRoot
        $searchFolders = "Client", "Server", "Shared"
        $files = Get-ChildItem -Path $searchFolders -Include *.c, *.cc, *.cpp, *.h, *.hh, *.hpp -Recurse -ErrorAction SilentlyContinue

        $tmp = [System.IO.Path]::GetTempFileName()
        $files.FullName | ForEach-Object { '"{0}"' -f $_ } | Set-Content $tmp -Encoding utf8
        Write-Verbose "List of files to format written to temporary file: $tmp"

        & "$clangFormatPath" @arguments "@$tmp"

        Remove-Item $tmp
        Write-Verbose "Successfully formatted $($files.Count) files."

        # Check git diff only on Linux (CI's checkout starts off clean)
        if ($isLinux) {
            $diffOutput = git diff --name-only
            if ($diffOutput.Count -eq 0) {
                Write-Host "No formatting changes detected." -ForegroundColor Green
                exit 0
            } else {
                Write-Host "::group::Files with formatting issues ($($diffOutput.Count) files)"
                $diffOutput | ForEach-Object { Write-Host $_ }
                Write-Host "::endgroup::"
                Write-Error "Code formatting issues detected. Run utils/clang-format.ps1 locally and commit the changes."
                exit 1
            }
        }
    } finally {
        Pop-Location
    }
}

Invoke-ClangFormat @args
