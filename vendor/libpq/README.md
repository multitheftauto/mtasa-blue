# libpq (PostgreSQL client library)

Used by `Server/dbconpg` so scripts can call `dbConnect("postgresql", ...)`.

Only the client library is used. MTA doesn't embed a PostgreSQL server. Server owners run their own PostgreSQL server
and MTA connects to it, the same way `dbConnect("mysql", ...)` works.

## What is here

- `include/`: `libpq-fe.h`, `postgres_ext.h`, `pg_config_ext.h` from PostgreSQL 17.6.
- `lib/x64/libpq.lib`: the import library for `libpq.dll`.
- `LICENSE`: the PostgreSQL License.

The runtime DLLs are in `Shared/data/MTA San Andreas/server/x64`, which `win-install-data.bat` copies into `Bin`:

| DLL | From | Licence |
|---|---|---|
| `libpq.dll` | PostgreSQL 17.6 | PostgreSQL License (BSD-like) |
| `libintl-9.dll` | GNU gettext, a dependency of libpq's message translations | LGPL 2.1+ (dynamically linked) |
| `libiconv-2.dll` | GNU libiconv, a dependency of libintl | LGPL 2.1+ (dynamically linked) |
| `libwinpthread-1.dll` | MinGW-w64 winpthreads, a dependency of libintl | MIT/BSD-style |

libpq also needs `libssl-3-x64.dll` and `libcrypto-3-x64.dll`. It uses the copies MTA already ships for MySQL
(OpenSSL 3.4.2, see `vendor/mysql/README.md`). Every OpenSSL function libpq 17.6 imports is exported by those DLLs.
Checked with `dumpbin /imports libpq.dll` against `dumpbin /exports` of MTA's OpenSSL.

## Where the binaries came from

The headers, `libpq.lib` and the four DLLs were copied from the official EDB Windows x64 installer of
PostgreSQL 17.6 (`C:\Program Files\PostgreSQL\17\include`, `lib` and `bin`).
Source: <https://github.com/postgres/postgres/tree/REL_17_6>.

Only x64 is provided. The Windows arm64 build of `dbconpg` is excluded, and `dbConnect("postgresql", ...)` returns
`false` with an error there. The 32-bit Windows server is no longer built.

## Follow-up: build from source

For an official release, build libpq from source like `vendor/mysql`, against MTA's own OpenSSL 3.4.2 build:

```bat
git clone --branch REL_17_6 https://github.com/postgres/postgres.git
cd postgres
meson setup build-x64 --buildtype=release -Dssl=openssl -Dnls=disabled -Dextra_include_dirs=<openssl>\include -Dextra_lib_dirs=<openssl>\lib
ninja -C build-x64 src/interfaces/libpq/libpq.dll
```

`-Dnls=disabled` removes the dependency on `libintl`/`libiconv`/`libwinpthread`, so only `libpq.dll` has to ship.
Repeat with an arm64 developer prompt to add arm64 support, then sign the DLLs.
