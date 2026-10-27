# Changelog

## Latest Patch ([v0.4.2]) - 2026-10-27

### Fixed
- Icon not being visible
- Program's style and caches not working after deleting source code

## Patch ([v0.4.1]) - 2026-10-25

### Fixed
- Empty categories showing up, even though they dont have available caches
- CLI version not showing system package manager, while GUI does
- Incorrect color enabling in terminal logging

## Stable release ([v0.4.0]) - 2026-10-25

### Added
- Clear report export to `JSON`
- Support for 95%+ Linux distributions package managers
- Button `clear trash`
- Cache sorts out by descending size
- Colored logging in terminal
- Changelog
- Animations and custom style
- `cmake --install` command support
- `cache-cleaner.desktop` file
- Logging to file
- User feedback in GUI (error messages)
- AUR package (PKGBUILD)
- Appimage

### Changed
- Improved UI: cache sorted in tables, custom style with animations
- Improved JSON parsing: more flexible config

### Fixed
- Crashes on incorrect JSON format
- Program crashing in some situations
- Clear report showing wrong command exit code
- Caches being cleared by paths, instead of by commands
- Not cleaning cache by commands properly
- Fixed config bugs: name/path dublicates not adding to the list properly and more