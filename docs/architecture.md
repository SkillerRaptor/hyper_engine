# HyperEngine Architecture

## Contents

1. [Hierarchy](#hierarchy)

## Hierarchy

```mermaid
graph TD;
    Editor-->Engine;

    Engine-->Game;
    Render-->Game;

    Engine-->Render;

    Engine-->Platform;
    Game-->Platform;
    Render-->Platform;

    Engine-->Core;
    Platform-->Core;
```
