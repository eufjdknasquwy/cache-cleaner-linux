# Cache cleaner

A utility for Linux to free disk space by cleaning caches of
package managers and user-added directories

## Table of contents

- [Status](#status)
- [Screenshots](#screenshots)
- [Features](#features)
- [How it works](#how-it-works)
- [Dependencies](#dependencies)
- [Building from source](#building-from-source)
- [Usage](#usage)
- [Configuration](#configuration)
- [Limitations](#limitations)
- [Known issues](#known-issues)
- [Roadmap](#roadmap)
- [License](#license)
- [Credits](#credits)

## Status

**Alpha (v0.3.0)** — supports `yay`, `paru`, `pip`, `go`, `nuget`, `npm`,
and system caches: `pacman`, `apt`, `dnf`, `flatpak`, `snap`, `journald`.
Supports GUI and CLI modes.

## Screenshots

![Main window](docs/screenshots/screenshot1.png)
![CLI](docs/screenshots/screenshot2.png)

## Features

### GUI
- Categories by danger level: Safe, Warning, Unknown, System, User
- Checkboxes for individual paths, "Choose everything" per category
- Confirmation dialogs for dangerous categories (double confirm for Warning/Unknown)
- User files are moved to trash

### CLI
- `--list` — list all caches
- `--delete-safe`, `--delete-warning`, `--delete-unknown`, `--delete-system`, `--delete-user`
- `-y` / `--yes`, `--dry-run` for scripts / testing
- `--tray` — run in tray (planned)

### Cache support
- AUR helpers: `yay`, `paru`
- Programming languages: `pip`, `go`, `nuget`, `npm`
- System package managers: `pacman`, `apt`, `dnf`, `flatpak`, `snap`
- System cache: `journalctl`

### Config
- `categories.json` — built-in
- `~/.config/cache-cleaner/user_categories.json` — user path

## How it works

1. Reads `categories.json` (built-in) and `~/.config/cache-cleaner/user_categories.json` (user)
2. Displays them in the window as checkboxes, grouped by danger level
3. When you click **Clear selected**:
   - **User caches** are moved to trash
   - **System caches** are cleared via `pkexec` (sudo command)
4. Confirmation dialogs for dangerous categories

## Dependencies

- A C++20 compiler (GCC 10+ or Clang 11+)
- CMake 3.16+
- nlohmann-json
- gtkmm-3.0 (is needed for GUI compilation)

## Building from source

```bash
git clone https://github.com/eufjdknasquwy/cache-cleaner-linux
cd cache-cleaner-linux
cmake -B build -S .
cmake --build build
```

## Usage

### GUI
1. Build the project (see above)
2. Run:
   ```bash
   ./build/cache-cleaner
   ```
3. Select the caches you want to clean and confirm.

### CLI
See --help of program to use CLI utils

## Configuration

`~/.config/cache-cleaner/user_categories.json` - path to user-added cache paths. Created automatically on the first run

Example:

```json
{
  "categories": [
    {
      "category": "User",
      "danger_level": "User",
      "entries": [
        { "name": "my-app", "path": "~/.cache/my-app", "type": "path" },
        { "name": "some-app", "path": "~/some-app", "type": "command", "command": "some-app --clean-cache" }
      ]
    }
  ]
}
```

## Limitations

- Linux only
- Root caches require polkit agent

## Known issues (problems)

- No fallback to `sudo` if `pkexec` is unavailable
- No support for many package managers

## Roadmap

### Done
- [x] Cleaning package manager caches like `pip`, `yay` and more
- [x] Cleaning System caches like `pacman` or `apt` and more
- [x] Systemd logs (`journalctl`)
- [x] Confirmation dialogs before deleting
- [x] Root-cleaning via `pkexec`
- [x] Basic CLI mode (`--list`, `--delete-x`, `--dry-run`, `--yes`)
- [x] Report after cleaning
- [x] Cache size showcase
- [x] Logging
- [x] `--dry-run`
- [x] `--yes` for scripts
- [x] Cache sorts out by descending
- [x] `Clear trash` button
- [x] Export clear report to JSON

### Planned
- [ ] GTK4 instead of GTK3
- [ ] Error messages in GUI
- [ ] Cleaning logs for not only systemd
- [ ] Scan size button
- [ ] Async operations for performance
- [ ] Custom CSS and animations
- [ ] Progress bar when deleting
- [ ] Ability to cancel deletion
- [ ] DE integration (notifications, tray)
- [ ] Cache searching in categories

## License

MIT - see [LICENSE](LICENSE).

## Credits

- [gtkmm](https://github.com/GNOME/gtkmm) - GUI
- [nlohmann/json](https://github.com/nlohmann/json) - JSON
