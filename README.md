# Cache cleaner

[![GitHub release](https://img.shields.io/github/v/release/eufjdknasquwy/cache-cleaner-linux)](https://github.com/eufjdknasquwy/cache-cleaner-linux/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
![C++](https://img.shields.io/badge/C%2B%2B-20-blue)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey)

A utility for Linux to free disk space by cleaning caches of package managers and other caches

## Table of contents

- [Status](#status)
- [Screenshots](#screenshots)
- [Features](#features)
- [How it works](#how-it-works)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Limitations](#limitations)
- [Known issues](#known-issues)
- [Roadmap](#roadmap)
- [License](#license)
- [Credits](#credits)

## Status

**Alpha (v0.4.0)** — supports `yay`, `paru`, `pip`, `go`, `nuget`, `npm`,
and system caches: `pacman`, `apt`, `dnf`, `flatpak`, `snap`, `journalctl`.
Supports GUI and CLI modes.

## Screenshots

![Main window](docs/screenshots/Main_window.png)
![CLI](docs/screenshots/CLI.png)

## Features

### GUI
- Categories by danger level
- Checkboxes for individual paths, displayed in a table
- Confirmations before deleting
- User files are moved to trash
- Clear trash button

### CLI
- `--list` — list all caches
- `--delete-safe`, `--delete-warning`, `--delete-unknown`, `--delete-system`, `--delete-user`
- `-y` / `--yes`, `--dry-run` for scripts / testing
- `--tray` — run in tray (planned)

### Cache support
- AUR helpers: `yay`, `paru`
- Programming languages such as `pip`, `go`, `nuget`, `npm`
- System package managers such as `pacman`, `apt`, `dnf`, `flatpak`, `snap`
- System cache and logs: `docker, podman, journalctl`

### Configuration
See - [Configuration](#configuration)

## How it works

1. Reads built-in and user json configs with cache paths
2. Displays them in the window as table rows, grouped by danger level
3. When you click **Clear selected**:
   - **Non-system caches** are moved to trash
   - **System caches** are cleared via `pkexec` (sudo)
4. Confirms before cleaning

## Dependencies

- A C++20 compiler (GCC 10+ or Clang 11+)
- CMake 3.16+
- nlohmann-json
- gtkmm-3.0

## Installation

### AUR (arch-based)
```bash
yay -S cache-cleaner
# or
paru -S cache-cleaner
```

### Other distributions (appimage)
1. Go to the [Releases](https://github.com/eufjdknasquwy/cache-cleaner-linux/releases) page.
2. Download the latest `cache-cleaner-x86_64.AppImage` file.
3. Open your terminal, go to the directory where you downloaded the file, and make it executable:
    ```bash
    chmod +x cache-cleaner-x86_64.AppImage
    ./cache-cleaner-x86_64.AppImage
    ```

### Other distribution (building from source)
```bash
git clone https://github.com/eufjdknasquwy/cache-cleaner-linux
cd cache-cleaner-linux
cmake -B build -S .
cmake --build build
sudo cmake --install build # for .desktop file in your system, and a terminal command
```

## Usage

### GUI
1. Install the program (see above)
2. Run:
   ```bash
   cache-cleaner
   ```
3. Select the caches you want to clean and confirm.

### CLI
See `cache-cleaner --help` in terminal

## Configuration

`~/.config/cache-cleaner/user_categories.json` - path to user-added cache paths. Created automatically on the first run

Example:

```json
{
  "categories": [
    {
      "entries": [
        { "name": "my-app", "desc": "cache of my app", "path": "~/.cache/my-app", "type": "path" },
        { "name": "some-app", "path": "~/some-app", "type": "command", "command": "some-app --clean-cache", "root": false }
      ]
    }
  ]
}
```

## Limitations

- Linux only
- Root caches require polkit agent
- Can't currently launch user's own scripts for cleaning cache

## Known issues

- No fallback to `sudo` if `pkexec` is unavailable
- No progress bar for delete operations
- No way to cancel deletion (except for Ctrl+C, which is bad)

## Roadmap

### Done
- [x] Cleaning package manager caches such as `pip`, `yay` and more
- [x] Cleaning system caches and system package managers such as `pacman`, `apt` and more
- [x] Cleaning Systemd logs (`journalctl`)
- [x] Confirmation dialogs before deleting
- [x] Root cache cleaning
- [x] CLI mode (`--list`, `--delete-x`, `--dry-run`, `--yes`)
- [x] Report after cleaning
- [x] Cache size showcase and sorting by descending size
- [x] Logging in terminal and to file
- [x] Colorful logging in terminal
- [x] `--dry-run` and `--yes` params for scripts
- [x] Export clear report to JSON
- [x] Custom style and animations
- [x] User feedback in GUI (error messages)

### Planned
- [ ] Port from GTK3 to GTK4
- [ ] Progress bar when deleting
- [ ] Ability to cancel deletion
- [ ] DE integration (notifications, tray)
- [ ] Search cache in categories
- [ ] Ability to add scripts for cleaning cache in user config

## License

MIT - see [LICENSE](LICENSE).

## Credits

- [gtkmm](https://github.com/GNOME/gtkmm) - GUI
- [nlohmann/json](https://github.com/nlohmann/json) - JSON