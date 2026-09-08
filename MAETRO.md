# maetro releases

Maetro is a special build of Multi Theft Auto for Windows 7, Windows 8, and Windows 8.1.

The name is a portmaneteau of [Windows Aero](https://en.wikipedia.org/wiki/Windows_Aero) and [Windows Metro](https://en.wikipedia.org/wiki/Windows_Metro).

## Differences between the regular MTA release and the maetro release

- maetro uses an older version of CEF, as Chrome [doesn't support](https://support.google.com/chrome/thread/185534985/sunsetting-support-for-windows-7-8-1-in-early-2023?hl=en) Windows 7, Windows 8, and Windows 8.1.
- maetro uses a different version of `d3dcompiler_47.dll`, stored [as `d3dcompiler_47.maetro.dll`](https://github.com/qaisjp/mtasa-blue/commits/master/Shared/data/MTA%20San%20Andreas/MTA/d3dcompiler_47.maetro.dll).
- various [other minor customisations](https://github.com/search?q=repo%3Amultitheftauto%2Fmtasa-blue%20MTA_MAETRO&type=code) to make sure the build continues to succeed

## Building

- maetro is built from the `master` branch with the `MTA_MAETRO=true` environment variable.
- Build maetro locally by running `$env:MTA_MAETRO='true'; .\win-create-projects.bat; \$env:MTA_MAETRO=$null` in PowerShell.
- GitHub Actions doesn't currently build maetro.
