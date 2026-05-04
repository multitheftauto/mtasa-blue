#### Summary

Follow-up to #4870. Move the weather re-apply earlier in `CGame::Process` so the engine's clock-wrap branch in `CWeather::Update` also runs against MTA's intended weather pair.

#### Motivation

#4870 fixed the sky/ambient flicker by re-applying MTA's weather in `PreWorldProcessHandler` (`0x53C095`), ahead of `CTimeCycle::CalcColoursForPoint` (`0x53C0DA`). Cloud cover, fog and night-time building lights still pop for the same frames, because `CWeather::Update` itself runs earlier (`0x53BFC2`) and on the wrap branch derives `Rain`, `Foggyness`, `CloudCoverage`, `ExtraSunnyness`, `SunGlare`, `HeatHaze`, `Sandstorm`, `WetRoads`, `Wind` and `Rainbow` from its own freshly-picked pair before `PreWorldProcessHandler` gets a chance to fix things.

This PR adds a `HookInstallCall` over the `call CWeather::Update` site at `0x53BFC2` and a new `PreWeatherUpdateHandler` that runs `CBlendedWeather::DoPulse` before the engine update. With Old/New/`InterpolationValue` already aligned, `CWeather::Update` takes the non-wrap branch and derives every weather global from MTA's pair. The existing `PreWorldProcessHandler` re-apply from #4870 is kept as an idempotent safety net for the colour-set path.

`CBlendedWeather::DoPulse` also calls `CWeather::ResyncInterpolationWithGameClock` after `Set`, so `InterpolationValue` matches the clock — fixes residual flicker on glass/reflective materials that #4870 left behind.

#### Test plan

1. Join a server running the default race resource
2. Set fullscreen (any mode)
3. Use `setWeatherBlended` + `setTime` via script, then restart the race resource to respawn
4. Observe no sky/lighting flicker on respawn
5. Repeat at night so building/streetlight glow and cloud cover are visible — they should not pop either

#### Checklist

* [x] Your code should follow the [coding guidelines](https://wiki.multitheftauto.com/index.php?title=Coding_guidelines).
* [x] Smaller pull requests are easier to review. If your pull request is beefy, your pull request should be reviewable commit-by-commit.
