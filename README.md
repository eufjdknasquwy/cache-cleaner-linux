# Cache cleaner

A graphical utility for Linux to free disk space by cleaning caches of
package managers and user-added directories.

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

**Alpha (v0.1.0)** — works for `yay` and `paru` caches. Root-cleaning, other
package managers, and confirmations are not implemented yet.

## Screenshots

![Main window](docs/screenshots/screenshot1.png)

## Features

- `yay` and `paru` cache cleaning
- custom directories cleaning

## How it works

1. Reads `categories.json` (built-in) and `~/.config/cache-cleaner/user_categories.json` (user).
2. Displays them in the window as checkboxes.
3. When you click **Clear selected**, moves selected caches to the system trash. Files can be restored.

## Dependencies

- A C++20 compiler (GCC 10+ or Clang 11+)
- CMake 3.16+
- gtkmm-3.0
- nlohmann-json

## Building from source

```bash
git clone https://github.com/eufjdknasquwy/cache-cleaner-linux
cd cache-cleaner-linux
cmake -B build -S .
cmake --build build
```

## Usage

1. Build the project (see above).
2. Run:
   ```bash
   ./build/cache-cleaner
   ```
3. Select the caches you want to clean and confirm.

## Configuration

`~/.config/cache-cleaner/user_categories.json` - path to user-added cache paths. Created automatically on the first run

Example:

```json
{
  "categories":
  [
    {
      "category": "User",
      "danger_level": "User",
      "paths":
      [
        "~/.cache/my-app",
        "~/.local/share/my-app/cache"
      ]
    }
  ]
}
```

## Limitations

- Requires a GTK environment (cannot be used in a terminal)

## Known issues

- Currently supports only `yay` and `paru`
- No confirmations for `Warning`, `Unknown`, categories
- Can't delete files owned by root
- Crashes on a corrupted `user_categories.json` (parsing error)

## Roadmap

- [x] Cleaning `yay` / `paru` caches
- [ ] Root-cleaning
- [ ] Package managers cache cleaning
- [ ] Systemd logs cleaning
- [ ] Confirmations for deleting dangerous files
- [ ] Fix crashes

## License

MIT - see [LICENSE](LICENSE).

## Credits

- [gtkmm](https://gtkmm.org/) — GUI
- [nlohmann/json](https://github.com/nlohmann/json) — JSON configurations
