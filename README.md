# Apollo (v2)

**Quick links:** [itch.io](https://yellowafterlife.itch.io/gamemaker-lua)
· [GM Marketplace](https://marketplace.yoyogames.com/assets/5192/_)

This extension lets you integrate Lua scripting in your GameMaker 2022+ projects!

This is version 2, while the (latest) version 3 can be found [here](https://github.com/YAL-GameMaker/Apollo).

Yet, you might still want V2 for existing projects or platforms without YYRunnerInterface support.

## What's interesting here

Apollo v2 takes advantage of (almost) everything in Lua being "coroutine-able" -
so whenever the Lua code needs something from GML, the state will yield (`lua_yieldk`),
store what it needs, and return execution back to GML, where we fetch what we were asked
and do a `lua_resume` afterwards.

This has a few limitations, but is enough for most uses.

## Asides

There's probably something that I forgot here, been a while since I had to touch V2.

## Building

See [BUILD.md](BUILD.md)

## Other FAQs

(see the [V3 README](https://github.com/YAL-GameMaker/Apollo/blob/main/README.md))

## Meta

**Author:** [YellowAfterlife](https://github.com/YellowAfterlife)  
**License:** Custom license (see `LICENSE`)
