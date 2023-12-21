# Farbrausch V2 Synthesizer System, Revived

> To whom it may concern,
> 
> Yes, this source code is a total mess. Good luck getting it to compile ... because the main VS project itself is in no shape to compile. But in theory all the source code for compiling the synth library and plugins is there, so it should be doable. :) ...
> 
> -- *Tammo "kb" Hinrichs, in [the original README](https://github.com/farbrausch/fr_public/blob/ca1d8dc/v2/README.txt)*

Well, *challenge accepted*. And here we are, after days of painfully digging through ancient codebases, taking it apart and porting pre-C++98 code for usage in 2023, I present to you a *working* ("functional" would be a strong word) fork of the original V2 codebase, including libv2, synth plugins and more, all with x64 support.

## So what do we have here?

- **libv2** for V2M playback in your applications, complete with RONAN (the speech synthesizer) and full x86/x64 support.
    - You may even be able to use it in some modern builds of other old Farbrausch tools, who knows ;)
- **v2player** for playing/sharing your V2M collection without needing to compile anything
- **V2 synth plugins for Buzz and VST** (projects "v2edit", "v2buzz" and "v2vsti")
    - The Buzz plugin is not in any sense tested.
    - Project "v2standalone" is a weird & probably deprecated standalone version of v2edit, which is only kept here for archival purposes.
- **V2 player plugin for Winamp** (project "in_v2m")
    - Not in any sense tested.
- **conv2m** fixed to correctly convert your old V2Ms to the current format
- Some other useful stuffs (like sample V2Ms and plugin skins) are kept in subdirectory "stuffs".

## What do we *not* have here?

- The assembly version of the V2 synth core is removed, and it's **not** coming back.
    - This do in theory make the code larger and run slower, but c'mon, who's gonna fix some decades-old unportable assembly code?
- The "tinyplayer" project is replaced by "v2player" mentioned above.
    - As above this codebase is no longer focused on small and efficent code. If you wished to use it in some intros then you're doing it wrong.
- RONAN is behaving erratically on old (Candytron-era) V2Ms. Untested if new(er) V2Ms are affected by this.
    - This problem is also in other source ports (e.g. [Chip Player JS](https://chiptune.app/)). I wonder if the public V2 code is just too new...
    - If you wish to take a look at this problem, ["fr_public"](https://github.com/farbrausch/fr_public) has lots of different versions of V2 code scattered everywhere in the repo, including [the version used by Candytron](https://github.com/farbrausch/fr_public/blob/ca1d8dc/genthree/ronan.cpp).
- **No further support or maintenance will ever be provided.**
    - I've got a copy of someone else's code, fixed it, compiled it, and that's it. Keep that in your mind if you want to fire a issue or anything.
    - Besides, v2edit's code is already butchered beyond belief to even look at...

## To build it yourself...

You would need Visual Studio 2022 or later as your IDE. NASM/YASM is not needed (I've removed all dependencies on them).

The plugins require several third-party SDKs to build, and you need to put them into corresponding directories. Information on where to get them can be found in "PUT_XXX_HERE.txt" files in subdirectories "vst2", "winamp", and "wtl".

## Okay, but what's the point?

Here is one: .

