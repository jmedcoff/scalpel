# Scalpel

A clipping plugin, with high resolution wave visualization for precise mixing/mastering work.

Why?
> Most contemporary clipping plugins for this use case (source: about 10 minutes of googling, some hours trying various options) either do not visualize or do not visualize with sufficient resolution. I want to be able to see the effects of clipping sample-by-sample, in an oscilloscope with some control over zoom and syncing.

## Tools

- Visual Studio Build Tools 2026 (or Visual Studio 2026 if you hate yourself)
  - C++ Desktop Development workload
  - Windows 10/11 SDK
- CMake 3.15 or higher
- Git

## Setup

```powershell
git clone <repository-url>
cd scalpel
git submodule update --init --recursive
```

```powershell
cmake -B build -G "Visual Studio 18 2026"
```

```powershell
cmake --build build --config Release
```

## Feature roadmap

### Done:
- Project setup

### To do:
- MVP: basic features
  - waveform visualizer - free or synced to DAW, uses left/right/mid input
  - hard clipper with input gain and output gain knobs, and "link" button which forces $gain_{out} = -gain_{in}$
  - window control: if synced, buttons let you choose a window size of (1/4, 1/2, 1, 2, 4, 8) measures; if free, a slider lets you dial a window size between 100 and 10000 milliseconds
- v2: improved visualization
  - RGB waveform display, like rekordbox or minimeters
  - clipping indication with distinct color
  - freeze button - stops updating display buffer, for examination of static content
  - zoom - mouse wheel on the visualization to zoom in; individual samples appear as small points
  - select - click and drag to select a region in the visualization, with readouts for duration of selection in ms, peak level inside selection, and RMS of selection
- v3: improved clipping
  - selectable clipping algorithms including soft saturation
  - add UI knob for soft saturation parameter
  - oversampling up to 16x
  - TBD?

## License

TBD, whatever JUCE mandates for free use of its framework.
