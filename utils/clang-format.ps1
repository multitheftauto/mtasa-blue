#!/usr/bin/env pwsh

$ToolConfig = @{
    "clang-format" = @{
        "linux-amd64" = @{
            "url" = "https://github.com/multitheftauto/clang-tools-static-binaries/releases/download/v21.1.7/clang-format-21_linux-amd64"
            "filename" = "clang-format"
            "hash" = "3d75779cdc69b06f7e39946b0e50d6ce3dccda1d766e355bf4cf507b1ae13303"
        }
        "windows-amd64" = @{
            "url" = "https://github.com/multitheftauto/clang-tools-static-binaries/releases/download/v21.1.7/clang-format-21_windows-amd64.exe"
            "filename" = "clang-format.exe"
            "hash" = "11defe615493876745f36fb77e9783e7eed03a7f800044ff58619d9293e79409"
        }
    }
    "clang-tidy" = @{
        "linux-amd64" = @{
            "url" = "https://github.com/multitheftauto/clang-tools-static-binaries/releases/download/v21.1.7/clang-tidy-21_linux-amd64"
            "filename" = "clang-tidy"
            "hash" = "7b35986c04b7c4fc7040c254ac2b5eb0b01ece87c91cccb32e3a4d6ecfa9695f"
        }
        "windows-amd64" = @{
            "url" = "https://github.com/multitheftauto/clang-tools-static-binaries/releases/download/v21.1.7/clang-tidy-21_windows-amd64.exe"
            "filename" = "clang-tidy.exe"
            "hash" = "e6afe42eb50ec9dcdf945d88b220b6b36971573cec7c5b6db22d3535c6cc61c4"
        }
    }
}

function Get-ClangTool {
    param(
        [Parameter(Mandatory)] [string]$RepoRoot,
        [Parameter(Mandatory)] [string]$ToolName
    )

    $platform = if ($isLinux) { "linux-amd64" } else { "windows-amd64" }
    $toolConfig = $ToolConfig[$ToolName][$platform]
    if (-not $toolConfig) {
        Write-Error "No configuration found for tool '$ToolName' on platform '$platform'"
        exit 1
    }

    $binDir = Join-Path $RepoRoot "Build" "tmp"
    if (-not (Test-Path $binDir)) {
        New-Item -ItemType Directory -Path $binDir | Out-Null
    }

    # Check existing file
    $toolPath = Join-Path $binDir $toolConfig.filename
    if (Test-Path $toolPath) {
        $currentHash = (Get-FileHash -Path $toolPath -Algorithm SHA256).Hash
        if ($currentHash -eq $toolConfig.hash) {
            return $toolPath
        }
        Write-Warning "$ToolName hash mismatch, re-downloading..."
    }

    # Download process
    Write-Host "Downloading $ToolName..." -ForegroundColor Cyan
    Invoke-WebRequest -Uri $toolConfig.url -OutFile $toolPath
    if ($isLinux) {
        chmod +x $toolPath
    }
    Write-Verbose "Downloaded $ToolName to $toolPath"

    # Verify hash
    $downloadedHash = (Get-FileHash -Path $toolPath -Algorithm SHA256).Hash
    if ($downloadedHash -ne $toolConfig.hash) {
        Remove-Item $toolPath -ErrorAction SilentlyContinue
        Write-Error "SHA256 hash mismatch! The download may be corrupted. Expected: $($toolConfig.hash) Got: $downloadedHash"
        exit 1
    }

    Write-Verbose "$ToolName verified successfully."
    return $toolPath
}

function Invoke-ClangFormat {
    [CmdletBinding()]
    param()

    $repoRoot = Split-Path -Parent $PSScriptRoot
    Push-Location $repoRoot
    Write-Verbose "Changed directory to repository root: $repoRoot"

    try {
        $clangFormatPath = Get-ClangTool -RepoRoot $repoRoot -ToolName "clang-format"
        $clangTidyPath = Get-ClangTool -RepoRoot $repoRoot -ToolName "clang-tidy"

        Write-Verbose "Searching for source files to format..."
        $searchFolders = "Client", "Server", "Shared"
        $files = Get-ChildItem -Path $searchFolders -Include *.c, *.cc, *.cpp, *.h, *.hh, *.hpp -Recurse -ErrorAction SilentlyContinue | Select-Object -ExpandProperty FullName

        $tmp = [System.IO.Path]::GetTempFileName()
        $files | Out-File $tmp -Encoding utf8
        Write-Verbose "List of files to format written to temporary file: $tmp"

        & "$clangFormatPath" -i --files=$tmp

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
