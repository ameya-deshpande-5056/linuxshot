# Contributing to LinuxShot

## Development Setup

1. Install dependencies:
   ```bash
   sudo apt install qt6-base-dev qt6-declarative-dev qt6-tools-dev cmake g++ git
   ```

2. Clone and build:
   ```bash
   git clone https://github.com/ameya-deshpande-5056/linuxshot.git
   cd linuxshot
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   cmake --build . -j$(nproc)
   ```

## Code Standards

- **Language**: C++20
- **Framework**: Qt 6
- **Build system**: CMake
- Use RAII and smart pointers — no raw owning pointers
- Separate UI code from business logic
- Use platform abstraction for X11/Wayland differences
- Handle errors explicitly — no silent failures
- Write unit tests for new functionality

## Module Structure

- `src/core/` — Core types, undo/redo, application lifecycle
- `src/capture/` — Screen capture backends (X11, Wayland/Portal)
- `src/annotation/` — Drawing tools and annotation engine
- `src/ui/` — QML interface and toolbar
- `src/platform/` — Platform abstraction layer
- `src/upload/` — Upload provider interface
- `src/settings/` — Configuration management

## Pull Request Guidelines

1. Create a feature branch from `main`
2. Write tests for new features
3. Ensure all existing tests pass
4. Update documentation as needed
5. Keep commits focused and well-described

## Versioning & Releases

- Follows [Semantic Versioning](https://semver.org/) (`MAJOR.MINOR.PATCH`).
- Always query the latest git tag (`git tag -l --sort=-v:refname | head -n 1`) before updating `CHANGELOG.md` or incrementing the project version.
- Updating `CHANGELOG.md` is mandatory for all releases; CI checks enforce this.
- See [`AGENTS.md`](AGENTS.md) for the complete 7-step release checklist.

## Reporting Issues

Include:
- Linux distribution and version
- Desktop environment (KDE, GNOME, Cinnamon, XFCE)
- Display protocol (X11 or Wayland)
- Steps to reproduce
- Expected vs actual behavior
- Screenshots if applicable

