# HyperEngine Architecture

## Contents

1. [Hierarchy](#hierarchy)

## Hierarchy

```mermaid
graph TD;
    Editor-->Engine;
    Engine-->Audio;
    Engine-->Game;
    Engine-->Physics;
    Engine-->Render;

    Render-->Render-Hal;
    Render-Hal-->Render-Core;

    Audio-->Math;
    Game-->Math;
    Physics-->Math;
    Render-Core-->Math;

    Audio-->Platform;
    Game-->Platform;
    Physics-->Platform;
    Render-Core-->Platform;

    Math-->Core;
    Platform-->Core;
```
