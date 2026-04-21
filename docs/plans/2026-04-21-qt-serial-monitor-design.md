# Qt Serial Monitor — Design Document

Date: 2026-04-21

## Overview

A cross-platform desktop serial port data monitor built with Qt 6.11 / C++17 / MSVC2022.
Primary purpose: Fiverr portfolio demo showcasing Qt/C++ GUI + real-time data visualization skills.

---

## Environment

- Qt 6.11, MSVC 2022
- Modules: Qt6::Widgets, Qt6::Charts, Qt6::SerialPort
- Build: CMake 3.16+
- Target platform: Windows (primary), cross-platform compatible

---

## Visual Style

### Color Palette

| Element            | Value     |
|--------------------|-----------|
| Window background  | `#F5F6FA` |
| Panel / card bg    | `#FFFFFF` |
| Primary (buttons)  | `#2563EB` |
| Connected state    | `#16A34A` |
| Disconnected state | `#6B7280` |
| Chart line         | `#2563EB` |
| Border / divider   | `#E5E7EB` |
| Primary text       | `#111827` |

### Typography
- System default sans-serif font
- Section headers in left panel: ALL CAPS small labels (e.g. `CONNECTION`, `SIGNAL`)
- Status indicator: colored dot + text label

---

## Layout

Window default size: 1000 × 640 px, resizable.

```
┌─────────────────────────────────────────────────────────────┐
│  ◈ Serial Monitor                    ● Connected  [Export CSV]│  ← Header bar
├─────────────────┬───────────────────────────────────────────┤
│ CONNECTION      │                                           │
│ Port  [Virtual▼]│     Real-time waveform (QChartView)       │
│ Baud  [9600  ▼] │     X-axis: time (s)  Y-axis: value       │
│ Bits  [8     ▼] │     Scrolling, last 200 points            │
│ Parity[None  ▼] │     Anti-aliased smooth curve             │
│ Stop  [1     ▼] │                                           │
│─────────────────┤───────────────────────────────────────────┤
│ SIGNAL          │ Timestamp        Value    ▲               │
│ ◉ Sine Wave     │ 12:01:03.421     0.832                    │
│ ○ Random Noise  │ 12:01:03.521     0.914                    │
│                 │ 12:01:03.621     0.763                    │
│ Freq  [──●──]   │ 12:01:03.721     0.651    [Clear Log]     │
│ 0.5Hz     5Hz   ├───────────────────────────────────────────┤
│                 │ Samples: 1,024 │ Min: 0.12 │ Max: 0.98   │
│ Amp   [──●──]   │ Rate: 10 Hz    │ Avg: 0.61 │             │
│ 0.1       5.0   │                                           │
│ [   Connect   ] │                                           │
└─────────────────┴───────────────────────────────────────────┘
```

### Panel Dimensions
- Left control panel: fixed width 200px
- Right area: chart 65% height, log panel 35% height
- Bottom status bar: single row, 4 stats fields

---

## Features

1. **Serial port configuration** — Port, Baud rate, Data bits, Parity, Stop bits dropdowns
2. **Built-in signal generator** — Sine wave or Random noise; adjustable frequency (0.5–5 Hz) and amplitude (0.1–5.0) via sliders
3. **Real-time scrolling chart** — Last 200 data points, 10 Hz refresh, smooth anti-aliased line
4. **Timestamped log panel** — Scrollable, appends each sample with `HH:mm:ss.zzz` timestamp; Clear button
5. **CSV export** — Exports all recorded samples (timestamp + value) via QFileDialog
6. **Status bar** — Live: sample count, sample rate, min/max/avg values

---

## Code Architecture

```
qt-serial-monitor/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── MainWindow.h / .cpp      ← Top-level window, assembles all widgets
│   ├── ControlPanel.h / .cpp    ← Left panel: connection params + signal generator
│   ├── ChartWidget.h / .cpp     ← QChartView wrapper, manages rolling data series
│   ├── LogPanel.h / .cpp        ← QPlainTextEdit wrapper, append + clear
│   └── DataGenerator.h / .cpp   ← QTimer-driven sine/random data source
└── docs/
    └── plans/
        └── 2026-04-21-qt-serial-monitor-design.md
```

### Class Responsibilities

**MainWindow**
- Creates and lays out ControlPanel, ChartWidget, LogPanel, status bar
- Connects signals between components
- Handles CSV export (QFileDialog + QFile write)
- Updates status bar stats on each new data point

**ControlPanel**
- Emits `connectRequested(bool)` when Connect button toggled
- Emits `signalTypeChanged(int)`, `freqChanged(double)`, `ampChanged(double)` on control changes
- Disables controls while connected

**ChartWidget**
- Holds a `QLineSeries` with max 200 points (FIFO)
- Slot `addPoint(double value, qint64 timestamp)` — appends and scrolls X axis
- Auto-scales Y axis with small padding

**LogPanel**
- Slot `appendEntry(QString timestamp, double value)`
- Button emits `clearRequested()`

**DataGenerator**
- `QTimer` at 100ms interval (10 Hz)
- Modes: `Sine` (uses elapsed time + freq + amp), `Random` (QRandomGenerator)
- Emits `dataReady(double value)`

### Signal Flow
```
DataGenerator::dataReady(value)
  → ChartWidget::addPoint()
  → LogPanel::appendEntry()
  → MainWindow: update stats (count, min, max, avg)
```

---

## Implementation Phases

### Phase 1 — Runnable skeleton (first milestone)
- CMakeLists.txt with Qt6 modules
- MainWindow with correct layout (panels, splitter, status bar)
- ControlPanel with all controls (non-functional dropdowns OK)
- ChartWidget showing live sine wave from DataGenerator
- LogPanel appending data
- Connect/Disconnect button toggles generator on/off

### Phase 2 — Full features
- CSV export
- Random noise mode
- Freq/Amp sliders wired up
- Status bar stats (min/max/avg/count/rate)
- Styling polish (colors, fonts, spacing)

---

## Build Instructions

```bash
# Requires Qt 6.11 + CMake 3.16+ + MSVC 2022
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/msvc2019_64"
cmake --build . --config Release
```

Or open `CMakeLists.txt` in Qt Creator directly.
