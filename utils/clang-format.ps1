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

    # Verify hash
    $downloadedHash = (Get-FileHash -Path $clangFormatPath -Algorithm SHA256).Hash
    if ($downloadedHash -ne $expectedHash.ToUpper()) {
        Remove-Item $clangFormatPath -ErrorAction SilentlyContinue
        Write-Error "SHA256 hash mismatch! Expected: $expectedHash, Got: $downloadedHash. The download may be corrupted."
        exit 1
    }

    Write-Verbose "clang-format downloaded and verified successfully."
    return $clangFormatPath
}

function New-GitDiffArtifact {
    Write-Host "Creating git diff artifact..." -ForegroundColor Cyan

    # Create git diff
    $diffOutput = git diff

    if ([string]::IsNullOrWhiteSpace($diffOutput)) {
        Write-Host "No formatting changes detected." -ForegroundColor Green
        return
    }

    # Save diff to file
    $artifactDir = "clang-format-diff"
    $diffFile = Join-Path $artifactDir "formatting.diff"

    if (-not (Test-Path $artifactDir)) {
        New-Item -ItemType Directory -Path $artifactDir | Out-Null
    }

    $diffOutput | Out-File $diffFile -Encoding utf8
    Write-Host "Diff saved to $diffFile" -ForegroundColor Green
    Write-Output "::set-output name=diff-created::true"
}

function Invoke-ClangFormat {
    [CmdletBinding()]
    param()

    $repoRoot = Split-Path -Parent $PSScriptRoot
    Push-Location $repoRoot
    Write-Verbose "Changed directory to repository root: $repoRoot"


    try {
        Write-Verbose "Searching for source files to format..."
        $clangFormatPath = Get-ClangFormat -RepoRoot $repoRoot
        $searchFolders = "Client", "Server", "Shared"
        $files = Get-ChildItem -Path $searchFolders -Include *.c, *.cc, *.cpp, *.h, *.hh, *.hpp -Recurse -ErrorAction SilentlyContinue | Select-Object -ExpandProperty FullName


        $tmp = [System.IO.Path]::GetTempFileName()
        $files | Out-File $tmp -Encoding utf8
        Write-Verbose "List of files to format written to temporary file: $tmp"

        & $clangFormatPath -i --files=$tmp

        Remove-Item $tmp
        Write-Verbose "Successfully formatted $($files.Count) files."

        # GitHub Actions should save an auto-fix
        if ($isLinux) {
            New-GitDiffArtifact
        }
    } finally {
        Pop-Location
    }
}

Invoke-ClangFormat @args
