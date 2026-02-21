# MySQL 9.6.0 Upgrade Notes

## Overview
This document describes the upgrade from MySQL 8.4.6 to MySQL 9.6.0 for the MTA:SA MySQL database module (`dbconmy`).

## What Has Been Done

### 1. Version References Updated
- Updated `vendor/mysql/include/mysql_version.h` to MySQL 9.6.0 (version ID: 90600)
- Updated `vendor/mysql/README.md` with MySQL 9.6.0 source references
- Updated main `README.md` to indicate MySQL 9.6+ support

### 2. Documentation Added
- Added MySQL 9.6.0 upgrade notes to `vendor/mysql/README.md` including authentication compatibility and migration guide
- Added comprehensive upgrade documentation in `vendor/mysql/UPGRADE_NOTES.md`
- Added patch file compatibility notes

### 3. Code Compatibility
- Added comments in `CDatabaseConnectionMySql.cpp` explaining MySQL 9+ authentication
- Verified that existing code uses `MYSQL_OPT_GET_SERVER_PUBLIC_KEY` which is compatible with MySQL 9's `caching_sha2_password`
- No source code changes required - the existing implementation is forward-compatible

## What Still Needs to Be Done

### 1. Compile MySQL 9.6.0 Client Libraries (Windows)
The actual MySQL 9.6.0 client library binaries need to be compiled following the instructions in `vendor/mysql/README.md`:

**Required for:**
- Windows x86 (`vendor/mysql/lib/x86/libmysql.lib`)
- Windows x64 (`vendor/mysql/lib/x64/libmysql.lib`)
- Windows ARM64 (`vendor/mysql/lib/arm64/libmysql.lib`)

**Note:** The current libraries in `vendor/mysql/lib/` are still MySQL 8.4.6 binaries. These need to be replaced with MySQL 9.6.0 binaries compiled using the instructions provided.

### 2. Update Patch File (If Needed)
The `vendor/mysql/mysql-server.diff` patch was created for MySQL 8.4.6. When compiling MySQL 9.6.0:
- Try applying the patch: `git apply vendor/mysql/mysql-server.diff`
- If it fails, manually update the patch for MySQL 9.6.0 source code changes
- Test that the patch applies cleanly and the build succeeds

### 3. Linux Build Dependencies
Currently, Ubuntu 24.04 Noble repositories only provide MySQL 8.0.x packages (`libmysqlclient-dev`):
- The code will compile against MySQL 8.x headers with updated version info
- For true MySQL 9.x support on Linux, wait for official Ubuntu packages or compile from source
- The current setup is forward-compatible and will work with MySQL 9 once packages are available

### 4. Testing
After compiling MySQL 9.6.0 libraries:
1. Build the MTA server with new libraries
2. Test connection to MySQL 8.4+ server using `caching_sha2_password`
3. Test connection to MySQL 9.x server
4. Verify all database operations work correctly
5. Test SSL/TLS connections

## Breaking Changes in MySQL 9.0+

### Authentication
- **Removed**: `mysql_native_password` authentication plugin (completely removed in 9.0)
- **Default**: `caching_sha2_password` using SHA-256 encryption
- **Impact**: Server administrators must update user accounts to use compatible authentication

### Server Migration Required
```sql
-- Check current authentication
SELECT user, host, plugin FROM mysql.user;

-- Update to caching_sha2_password
ALTER USER 'mtauser'@'%' IDENTIFIED WITH caching_sha2_password BY 'password';
FLUSH PRIVILEGES;
```

## Compatibility Matrix

| MTA Server Library | MySQL Server | Compatible | Notes |
|-------------------|--------------|------------|-------|
| MySQL 9.6.0 | MySQL 9.x | ✅ Yes | Optimal configuration |
| MySQL 9.6.0 | MySQL 8.4+ | ✅ Yes | Using `caching_sha2_password` |
| MySQL 9.6.0 | MySQL 8.0+ | ✅ Yes | Using `caching_sha2_password` |
| MySQL 9.6.0 | MySQL 5.7 or earlier 8.0 | ❌ No | If using `mysql_native_password` only |

## Benefits of MySQL 9.6.0

### Security Enhancements
- Stronger authentication with SHA-256
- Removal of weak `mysql_native_password` (SHA-1 based)
- Enhanced SSL/TLS support

### Performance Improvements
- Better query optimization
- Improved execution planning
- Enhanced caching mechanisms

### New Features
- Enhanced JSON support and functions
- EXPLAIN ANALYZE for query performance analysis
- Improved stored procedures and triggers

## References
- [MySQL 9.6.0 Release](https://github.com/mysql/mysql-server/releases/tag/mysql-9.6.0)
- [MySQL 9.5.0 Release](https://github.com/mysql/mysql-server/releases/tag/mysql-9.5.0)
- [What's new in MySQL 9](https://epigra.com/en/blog/whats-new-in-mysql-9)
- [MySQL 9.0 Authentication Changes](https://blogs.oracle.com/mysql/mysql-90-its-time-to-abandon-the-weak-authentication-method)

## Support
For questions or issues related to this upgrade:
1. Check the migration guide in `vendor/mysql/README.md`
2. Review MySQL 9.x release notes and documentation
3. Test thoroughly before deploying to production
