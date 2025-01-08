DLL files can be found in `Shared/data/MTA San Andreas/server`.
That directory gets copied to the "Bin" directory when you run "win-install-data.bat".

The source code for MySQL and OpenSSL used to produce the binaries can be found here:  
https://github.com/mysql/mysql-server/releases/tag/mysql-8.4.3  
https://github.com/openssl/openssl/releases/tag/openssl-3.4.0  

## How to compile OpenSSL

1. Install [Perl](https://strawberryperl.com/).
2. Install [NASM](https://www.nasm.us/).
3. Ensure both Perl and NASM are available on your `%PATH%`.
4. Clone the repository:  
    ```bat
    git clone --recurse-submodules https://github.com/openssl/openssl.git
    cd openssl
    ```
5. Switch to the release tag:  
    ```bat
    git switch --detach --force --recurse-submodules openssl-3.3.1
    ```
6. Run `VsDevCmd.cmd` to open three Developer Command Prompts.  
> [!IMPORTANT]  
> The next steps assume the OpenSSL checkout directory is `C:\GitHub\openssl`.
7. Switch to the `VsDevCmd: x64` console window and run these commands there:  
    ```bat
    cd C:\GitHub\openssl
    perl Configure --release --prefix="%cd%\VC-WIN64A\OpenSSL" --openssldir="%cd%\VC-WIN64A\SSL" /MT VC-WIN64A
    nmake
    nmake install
    ```
8. Switch to the `VsDevCmd: x86` console window and run these commands there:  
    ```bat
    cd C:\GitHub\openssl
    git clean -f -x
    perl Configure --release --prefix="%cd%\VC-WIN32\OpenSSL" --openssldir="%cd%\VC-WIN32\SSL" /MT VC-WIN32
    nmake
    nmake install
    ```
9. Switch to the `VsDevCmd: arm64` console window and run these commands there:  
    ```bat
    cd C:\GitHub\openssl
    git clean -f -x
    perl Configure --release --prefix="%cd%\VC-WIN64-ARM\OpenSSL" --openssldir="%cd%\VC-WIN64-ARM\SSL" /MT VC-WIN64-ARM
    nmake
    nmake install
    ```

> [!IMPORTANT]  
> Copy `VC-WIN64A\OpenSSL\include\openssl\applink.c` to `VC-WIN64-ARM\OpenSSL\include\openssl\`

## How to compile MySQL

1. Compile OpenSSL with the steps above.
2. Install [CMake](https://cmake.org/download/).
3. Clone the repository:  
    ```bat
    git clone --recurse-submodules https://github.com/mysql/mysql-server.git
    cd mysql-server
    ```
4. Switch to the release tag:  
    ```bat
    git switch --detach --force --recurse-submodules mysql-8.4.0
    ```
5. Apply the patch file:  
    ```bat
    git apply path\to\vendor\mysql\mysql-server.diff
    ```
6. Run `VsDevCmd.cmd` to open three Developer Command Prompts.  
> [!IMPORTANT]  
> The next steps assume the OpenSSL checkout directory is `C:\GitHub\openssl`  
> and the MySQL checkout directory is `C:\GitHub\mysql-server`.  
> You must always compile x64 before cross-compiling arm64.
7. Switch to the `VsDevCmd: x64` console window and run these commands there:  
    ```bat
    cd C:\GitHub\mysql-server
    cmake -G "Visual Studio 17 2022" -A x64 -B build-x64 -DCMAKE_INSTALL_PREFIX="%cd%\install-x64" -DWITH_SSL="C:\GitHub\openssl\VC-WIN64A\OpenSSL"
    cmake --build build-x64 --target INSTALL --config RelWithDebInfo
    ```
8. Switch to the `VsDevCmd: x86` console window and run these commands there:  
    ```bat
    cd C:\GitHub\mysql-server
    cmake -G "Visual Studio 17 2022" -A Win32 -B build-x86 -DCMAKE_INSTALL_PREFIX="%cd%\install-x86" -DWITH_SSL="C:\GitHub\openssl\VC-WIN32\OpenSSL"
    cmake --build build-x86 --target INSTALL --config RelWithDebInfo
    ```
9. Switch to the `VsDevCmd: arm64` console window and run these commands there:  
    ```bat
    cd C:\GitHub\mysql-server
    cmake -G "Visual Studio 17 2022" -A ARM64 -B build-arm64 -DCMAKE_INSTALL_PREFIX="%cd%\install-arm64" -DWITH_SSL="C:\GitHub\openssl\VC-WIN64-ARM\OpenSSL"
    cmake --build build-arm64 --target INSTALL --config RelWithDebInfo
    ```

## How to update MySQL

1. Compile MySQL with the steps above.
2. Copy each folder `C:\GitHub\mysql-server\install-{x86,x64,arm64}\` to a temporary directory.
3. Copy `install-x64\LICENSE` to `vendor\mysql`.
4. Copy `install-x64\include` to `vendor\mysql\include`.
5. For each `install-{arch}` folder:  
    1. Delete all folders, but keep `bin` and `lib`.
    2. In `lib` folder delete everything, but keep `libmysql.{dll,lib,pdb}`.
    3. In `bin` folder delete everything, but keep `libcrypto-*` and `libssql-*` files.
    4. Sign all `*.dll` files.
    5. Copy signed `*.dll` files to their designed subfolder in `Shared\data\MTA San Andreas\server`.
    6. Copy `lib\libmysql.lib` to `vendor\mysql\lib\{arch}\`.
6. Commit the update.
