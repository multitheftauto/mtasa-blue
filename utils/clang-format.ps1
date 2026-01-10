#!/usr/bin/env pwsh

function Get-ClangFormat {
    param(
        [Parameter(Mandatory)] [string]$RepoRoot,
        [string]$Version = "21.1.8"
    )

    if ($isLinux) {
        $clangFormatUrl = "https://github.com/multitheftauto/llvm-project/releases/download/llvmorg-${Version}/clang-format-${Version}-armv7a-linux-gnueabihf"
        $clangFormatFilename = "clang-format"
        $expectedHash = "6cb0689cd3288568690d1e681165e8c715d04498d007f7c9310a00d154e50d70"
    } else {
        $clangFormatUrl = "https://github.com/multitheftauto/llvm-project/releases/download/llvmorg-${Version}/clang-format-${Version}-x86_64-pc-windows-msvc.exe"
        $clangFormatFilename = "clang-format.exe"
        $expectedHash = "b77927335fd4caece863ea0c09ec8c4dfbdab545510c9091205ccfb528d5abf2"
    }

    $binDir = Join-Path $RepoRoot "Build" "tmp"
    $clangFormatPath = Join-Path $binDir $clangFormatFilename

    if (Test-Path $clangFormatPath) {
        $currentHash = (Get-FileHash -Path $clangFormatPath -Algorithm SHA256).Hash
        if ($currentHash -eq $expectedHash) {
            return $clangFormatPath
        }
        Write-Warning "clang-format hash mismatch, re-downloading..."
    }

    Write-Host "Downloading clang-format..." -ForegroundColor Cyan
    if (-not (Test-Path $binDir)) {
        New-Item -ItemType Directory -Path $binDir | Out-Null
    }

    Invoke-WebRequest -Uri $clangFormatUrl -OutFile $clangFormatPath
    if ($isLinux) {
        chmod +x $clangFormatPath
    }

    return $clangFormatPath
}

function Invoke-ClangFormat {
    [CmdletBinding(SupportsShouldProcess = $true)]
    param()

    $repoRoot = Split-Path -Parent $PSScriptRoot
    Push-Location $repoRoot

    try {
        $clangFormatPath = Get-ClangFormat -RepoRoot $repoRoot
        $searchFolders = "Client", "Server", "Shared"
        $files = Get-ChildItem -Path $searchFolders -Include *.c, *.cc, *.cpp, *.h, *.hh, *.hpp -Recurse -ErrorAction SilentlyContinue | Select-Object -ExpandProperty FullName

        if (-not $files) {
            Write-Host "No files found to format."
            return
        }

        if ($PSCmdlet.ShouldProcess("$($files.Count) files", "Format source code using clang-format")) {
            $tmp = [System.IO.Path]::GetTempFileName()
            $files | Out-File $tmp -Encoding utf8

            & $clangFormatPath -i --files=$tmp

            Remove-Item $tmp
            Write-Host "Successfully formatted $($files.Count) files." -ForegroundColor Green
        }
    } finally {
        Pop-Location
    }
}

Invoke-ClangFormat
