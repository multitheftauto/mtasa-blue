#### Summary

Re-apply MTA's blended weather state both **before** `CWeather::Update` and before `CTimeCycle::CalcColoursForPoint`, so neither the engine's clock-wrap branch nor the colour calculation ever observes a stale weather pair.

#### Motivation

Fixes #4803. When `setWeatherBlended`/`setTime` are used (e.g. race resource restart/respawn), the sky and ambient lighting flicker for 1-2 frames in fullscreen, and night-time building lights / cloud / fog also pop for the same frames.

The engine's `CWeather::Update` (called at `0x53BFC2` from `CGame::Process`) detects a clock wrap when `setTime` jumps the game clock past its internal `InterpolationValue`. It then overwrites `OldWeatherType`/`NewWeatherType` with its own weather-list pick **and** uses that wrong pair to derive `Rain`, `Foggyness`, `CloudCoverage`, `ExtraSunnyness`, `SunGlare`, `HeatHaze`, `Sandstorm`, `WetRoads`, `Wind` and `Rainbow` for the frame. `CalcColoursForPoint` (at `0x53C0DA`) then reads `OldWeatherType`/`NewWeatherType`/`InterpolationValue` to compute the sky/ambient colour set.

MTA's `CBlendedWeather::DoPulse` previously only ran in `DoPulses` during `Render2dStuff`, which is after the frame was already rendered with the wrong state.

Two hooks are now used:

1. **`HOOK_CWeather_Update`** — replaces the `call CWeather::Update` site at `0x53BFC2`. Fires `PreWeatherUpdateHandler` *before* the engine's update, so MTA's `Set` + `ResyncInterpolationWithGameClock` re-sync `OldWeatherType`/`NewWeatherType`/`InterpolationValue` with the new clock. `CWeather::Update` then takes the non-wrap branch and derives `Rain`/`Foggyness`/`CloudCoverage`/etc. from MTA's intended pair. This is what fixes the **building light / cloud / fog flicker**.
2. **`PreWorldProcessHandler`** (the original hook from #4870, kept as a defensive idempotent re-apply) — runs at `CWorld::Process` (`0x53C095`), still ahead of `CalcColoursForPoint` at `0x53C0DA`, so the sky/ambient frame is also guaranteed to use MTA's weather pair even if the early hook is bypassed for any reason.

Fullscreen-only because exclusive mode bypasses DWM composition, so every frame including the glitch frames is presented directly. Windowed mode's DWM triple buffering absorbs them.

#### Test plan

1. Join a server running the default race resource
2. Set fullscreen (any mode)
3. Use `setWeatherBlended` + `setTime` via script, then restart the race resource to respawn
4. Observe no sky/lighting flicker on respawn
5. Repeat at night-time so building/streetlight glow and cloud cover are visible — they should not pop either

#### Checklist

* [x] Your code should follow the [coding guidelines](https://wiki.multitheftauto.com/index.php?title=Coding_guidelines).
* [x] Smaller pull requests are easier to review. If your pull request is beefy, your pull request should be reviewable commit-by-commit.
