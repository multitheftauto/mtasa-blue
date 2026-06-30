# Sync Manipulation Guard (On-Foot)

## Why this exists

Some cheats inject client-side Lua that forges **outgoing player puresync** (position and/or velocity) while the attacker is **on foot**.
The server would otherwise accept and relay that sync to other clients, where the remote attacker ped is simulated with impossible movement and violently collides with nearby vehicles and players.

Typical attack pattern:

- attacker stays on foot
- attacker forges `api.sync.velocity(...)` (or equivalent `setElementVelocity` path)
- optionally forges `api.sync.position(...)` in the same or following packets
- velocity is only sent in some packets (`bSyncingVelocity`); position is sent every packet
- other clients receive and simulate that remote ped movement
- nearby vehicles are physically launched

This guard rejects forged on-foot puresync at the **server packet read stage**, before state is updated or packets are relayed.

## What is fixed

### 1) On-foot velocity sanity check

For player pure sync, the server rejects packets with impossible on-foot velocity magnitude:

- setting: `player_max_sync_speed` (km/h)
- default: `360`
- `0` disables the check

### 2) On-foot position delta check

Cheats can omit the velocity field and forge position only. Every on-foot puresync packet is checked against a reference position:

- last accepted puresync position when available
- otherwise the server's current element position (no unchecked "first packet" window)

When the player has **valid vehicle contact** (`bHasContact` on a vehicle that passed the existing contact radius checks), the delta is measured in **contact-relative** coordinates instead of absolute world space. That avoids false positives when surfing on a legitimately fast vehicle, where absolute movement reflects vehicle speed rather than on-foot ped speed.

The packet is rejected if the chosen reference position could not have reached the new position in the elapsed time at `player_max_sync_speed` (with a small margin). When no prior baseline exists, elapsed time defaults to one pure-sync tick (`iPureSync`, typically 100 ms).

### 3) Non-finite position rejection (NaN/Inf)

Player and vehicle pure sync position fields are rejected when coordinates are non-finite.

### 4) Main thread and sim thread parity

Player puresync is validated in both paths:

- main thread: `CPlayerPuresyncPacket::Read`
- sim thread relay: `CSimPlayerPuresyncPacket::Read`

The sim thread reads the last accepted baseline from the real `CPlayer` before validating so both paths agree on accept/reject.

Rejected packets are neither applied to server state nor relayed to other players.

## Legitimate bypasses

- **Server-authorized teleport:** `SetTeleported(true)` (spawn, `setElementPosition`, etc.) skips movement checks for the next accepted packet and clears the movement baseline.
- **In vehicle (seated):** on-foot checks apply only when the player is on foot or exiting a vehicle; vehicle gameplay is not speed-capped by this feature.
- **Vehicle contact (surf / standing on roof):** position delta uses contact-relative coordinates so fast vehicles remain playable.

## Important scope note

This guard targets **on-foot manipulation used to launch other players' vehicles and players**.

It does **not** introduce a general vehicle top speed limiter:

- `vehicle_max_sync_speed` is not used
- legitimate high-speed vehicle gameplay (500–1000+ km/h) is unaffected

## Configuration

In `mtaserver.conf`:

```xml
<player_max_sync_speed>360</player_max_sync_speed>
```

Guidance:

- Lower value: stricter anti-manipulation, higher false-positive risk
- Higher value: more permissive, weaker protection
- `0`: disabled (not recommended on public servers)

## Residual limits (why not mathematically perfect)

- **Threshold model:** The server does not run GTA physics; it sanity-checks client sync against a max on-foot speed. A cheater who stays just under 360 km/h (and the 1.35× margin) could still nudge movement slightly — that is a fundamental trade-off vs. jetpack, knockback, and high-ping jitter.
- **Authorized teleports:** `SetTeleported(true)` must bypass checks or legitimate warps break.
- **Compromised client binary:** A patched client can send anything; the meaningful fix is server-side relay rejection, not trusting the client.
- **`player_max_sync_speed = 0`:** Disables all checks.

For the documented on-foot sync-forge launch exploit (R/E scripts, position-only forge, vehicle-exit reset tricks), every relay path now runs velocity + position delta validation before apply/relay.

## Non-goals

- Not a full anti-teleport system for all movement edge cases
- Does not replace broader anti-cheat layers
- Client-side clamping is optional defense-in-depth; server validation is the primary fix
