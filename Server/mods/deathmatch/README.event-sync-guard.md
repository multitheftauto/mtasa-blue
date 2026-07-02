# Lua event sync guard (internal)

Hardens `PACKET_ID_LUA_EVENT` against verbatim replay and protects server→client
event delivery from Lua-level cancellation.

## Design constraint: the client is open source

MTA:SA's client is public. Any client-side check, key, hash or "signature" can be
read on GitHub and reproduced by a recompiled cheat, so **no security-through-obscurity
is used here**. The guarantees below rely only on:

1. **Server-side state the client cannot observe or rewind** (the expected sequence), and
2. **Server-side / logical decisions** (who may cancel an in-flight event).

A fully recompiled malicious client is out of scope — that is true for every check in an
open-source client and cannot be solved with client-side secrets.

## Threat model

| Vector | Mechanism | Guard | Open-source-proof? |
|--------|-----------|-------|--------------------|
| `ResendPacket` / packet filter | Resends a captured `PACKET_ID_LUA_EVENT` bitstream verbatim | The resent packet carries a stale sequence; server expects a higher one and drops it | Yes — expected value lives only on the server |
| `BlockedServerEventList` / `skip_call` on `TriggerServerEvent` | Drops selected client→server events before they leave the client | Not fixable server-side (packet never sent). Existing `max_player_triggered_events_per_interval` still applies to packets that do arrive | N/A |
| Injected script + `cancelEvent` | Cancels handlers for a server-triggered `triggerClientEvent` | During the remote pulse only the handler's own VM may cancel; foreign VMs are ignored | Yes — logical ownership check |
| `addDebugHook` preEvent `"skip"` | Blocks the whole client event dispatch | preEvent/preEventFunction debug hooks are ignored while dispatching server remote events | Yes — logical check |

## Anti-replay protocol

- Bitstream feature: `eBitStreamVersion::ClientLuaEventSequence`.
- The client appends a monotonically increasing `uint32` after the Lua arguments on each
  `triggerServerEvent` / `triggerLatentServerEvent`.
- The server tracks the next expected sequence per player and rejects any packet whose
  sequence is **below** it (monotonic, not strict-equality, so reliable-ordered and latent
  events interleaving never drops a legitimate packet).
- Legitimate duplicate payloads stay valid — e.g. calling
  `triggerServerEvent("test", localPlayer, "same")` twice sends two distinct sequences.
- A modern client (one whose bitstream version supports the feature) **must** include the
  sequence; a packet without it was not produced by stock code and is rejected.

### What this does and does not stop

- **Stops:** dumb capture-and-resend (the cheat's `ResendPacket` path), which is the actual
  observed attack.
- **Does not stop:** a cheat that bumps the sequence before resending. But that is no longer
  a "replay" — it is indistinguishable from the script simply calling `triggerServerEvent`
  again, which is already legal. So nothing is lost.

## Server→client cancellation guard

When the server delivers a remote Lua event (`CPacketHandler::Packet_LuaEvent`), the client
enters a "remote event pulse":

- `cancelEvent` is accepted only from the VM that registered the currently running handler,
  so an injected resource cannot cancel another resource's server-triggered handler.
- `CDebugHookManager::OnPreEvent` / `OnPreEventFunction` ignore `"skip"` during the pulse,
  so a debug-hook based blocker cannot suppress delivery.

This is a logical guard, not a secret, so being open source does not weaken it.

## Key files

- `Server/.../CPlayer::TryAcceptClientLuaEvent` — monotonic sequence gate
- `Server/.../CGame::Packet_LuaEvent` — validation gate
- `Client/.../CStaticFunctionDefinitions.cpp` — appends the sequence on send
- `Client/.../CPacketHandler::Packet_LuaEvent` — remote event pulse
- `Client/.../CEvents.cpp` — scoped `cancelEvent` ownership
- `Shared/.../CDebugHookManager.cpp` — skips debug-hook blocking during the remote pulse

## Testing

1. Build client + server with matching bitstream version.
2. `triggerServerEvent("test", localPlayer, "same")` twice → both reach the server.
3. Capture and resend a `PACKET_ID_LUA_EVENT` packet verbatim → server drops the replay.
4. Server `triggerClientEvent` to a handler while a foreign script calls `cancelEvent` → the
   intended handler still runs.
