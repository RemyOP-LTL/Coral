# Coral — Arduino Project

## Overview
Biomimetic coral sculpture with ~1000 motorized polyps that open/close like blooming flowers. Currently in prototyping phase.

## Architecture (End Goal)
- **Scale:** ~1000 polyps on a large coral sculpture
- **Control:** Pixlite pixel controller driving everything over APA102 protocol
- **Per-polyp custom PCB:**
  - MCU that sniffs APA102 pixel data from daisy chain
  - 28BYJ-48 stepper motor + ULN2003 driver (drives the polyp mechanism)
  - RGB LEDs
  - Translates pixel color data into motor position/movement
- **Topology:** Custom PCBs daisy-chained together; looks like addressable LEDs to the Pixlite
- **Software:** Eventually a program that pushes lighting + motion effects to the pixel controller

## Prototype Setup (Current Phase)
- **Board:** Arduino Mega 2560
- **Motor:** 28BYJ-48 stepper + ULN2003 driver
- **Goal:** Get the polyp open/close mechanism working
- **GitHub:** https://github.com/RemyOP-LTL/Coral
- **Arduino sketch:** `Coral.ino`

## Workflow
- Always commit and push changes to GitHub automatically after edits.
- Explain concepts as you go — the user is learning.

## Current Status
- Initial project scaffold created (2026-04-02)
- Next step: prototype single polyp open/close with 28BYJ-48 stepper motor
