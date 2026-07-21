# Weapon Sync Guard (Bullet Sync + Aim Sync)

## Why this exists

Cheats hook client `SendBulletSync` and can:

- send many bullet-sync packets per physical shot (`DamageDividier` loops)
- forge start/end vectors (including non-finite or extreme trajectories)
- bypass `onPlayerWeaponFire` on the server while still crashing remote clients

Root cause: the **sim thread** relays `PACKET_ID_PLAYER_BULLETSYNC` before the main thread runs full validation. Invalid packets were broadcast to nearby players even when `CBulletsyncPacket::Read()` later rejected them.

Weapon aim data in puresync/keysync can also crash remote clients when origin ≈ target (division by zero in `SetTargetTarget`).

## What is fixed

### 1) Player-relative geometry (`SyncBulletsyncValidation.h`)

Validation is derived from the shooter sync position and weapon stats:

- **Muzzle origin** within a physical envelope around the ped/vehicle sync point (5 m on foot, 25 m in vehicle)
- **Bullet segment** length ≤ weapon range (+ lag tolerance)
- **Impact point** reachable from shooter: `origin_offset + weapon_range`
- Non-finite vectors and degenerate segments rejected

### 2) Sim-thread parity (`CSimPlayerManager::HandleBulletSync`)

Sim relay runs the same checks as `CBulletsyncPacket::Read()` **before** broadcasting:

- spawned/alive player, weapon ownership + ammo
- player-relative geometry + damage validation
- per-player rate limit (25 packets / second)

### 3) Weapon aim validation (puresync + keysync)

Full aim sync rejected when origin/target would crash remote simulation. Applied on main and sim relay paths.

### 4) Client defense in depth

- `CNetAPI::ReadBulletsync` drops invalid geometry
- `CClientPed::SetTargetTarget` ignores invalid/degenerate aim data

## Residual limits

- Rate limit is per-second, not per-weapon fire rate from stats
- Compromised clients can still send sub-threshold values; server relay rejection is the meaningful fix
- Custom weapon bullet sync validates on main thread only (no sim fast path)
