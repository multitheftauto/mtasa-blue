DLL files can be found in `Shared/data/MTA San Andreas/server`.
That directory gets copied to the "Bin" directory when you run "win-install-data.bat".

## MySQL 9.6.0 Upgrade Notes

This module has been upgraded to support MySQL 9.6.0 (Innovation track), bringing:
- **Enhanced Security**: Removed `mysql_native_password` authentication (deprecated in 8.4, removed in 9.0)
- **Default Authentication**: Now uses `caching_sha2_password` with SHA-256 encryption
- **Improved Performance**: Better query optimization and execution
- **New Features**: Enhanced JSON support, EXPLAIN ANALYZE, and VECTOR data type support

### Authentication Compatibility
MySQL 9+ no longer supports `mysql_native_password`. The client library automatically uses `caching_sha2_password` by default. Server administrators must ensure user accounts use compatible authentication methods:
```sql
ALTER USER 'username'@'host' IDENTIFIED WITH caching_sha2_password BY 'password';
```

The source code for MySQL and OpenSSL used to produce the binaries can be found here:  
https://github.com/mysql/mysql-server/releases/tag/mysql-9.6.0  
https://github.com/openssl/openssl/releases/tag/openssl-3.4.2  

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
    git switch --detach --force --recurse-submodules openssl-3.4.2
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
    git switch --detach --force --recurse-submodules mysql-9.6.0
    ```
5. Apply the patch file:  
    ```bat
    git apply path\to\vendor\mysql\mysql-server.diff
    ```
    > [!NOTE]  
    > The patch file was originally created for MySQL 8.4.6. When compiling MySQL 9.6.0, 
    > you may need to adjust line numbers or update the patch if the source files have changed.
    > The patch includes fixes for:
    > - Static runtime linking configuration
    > - 32-bit x86 build support
    > - OpenSSL detection skipping
    > - ARM64 cross-compilation support
    > - MYSQL_OPT_RECONNECT deprecation warning suppression
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

## Server Administrator Migration Guide

When upgrading an MTA server to use MySQL 9.6+, server administrators need to ensure their MySQL server configuration is compatible:

### Authentication Method Update
MySQL 9.0+ has removed `mysql_native_password` authentication. All database users must use `caching_sha2_password`:

```sql
-- Check current authentication plugins
SELECT user, host, plugin FROM mysql.user;

-- Update users to use caching_sha2_password
ALTER USER 'mtauser'@'%' IDENTIFIED WITH caching_sha2_password BY 'password';
FLUSH PRIVILEGES;
```

### Connection Requirements
- Ensure MySQL server is version 8.0+ (MySQL 9.x is backwards compatible with MySQL 8.x clients)
- If using SSL/TLS, ensure OpenSSL 3.4.2+ is installed
- The `get_server_public_key=1` option (enabled by default) allows secure password exchange

### Compatibility Notes
- MTA server with MySQL 9.6+ client library **can connect to** MySQL 8.x servers (using `caching_sha2_password`)
- MTA server with MySQL 9.6+ client library **cannot connect to** MySQL servers using `mysql_native_password` only
- For optimal security, upgrade both the MTA server's client library and the MySQL server to version 9.x
