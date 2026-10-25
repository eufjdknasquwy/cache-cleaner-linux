# Changelog

## Latest version ([v0.4.1]) - 2026-10-25

### Fixed
- Empty categories showing up, even though they dont have available caches
- CLI version not showing system package manager, while GUI does
- Incorrect color enabling in terminal logging

## Version ([v0.4.0]) - 2026-10-25

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
- Fixed crashes on incorrect JSON format
- Improved JSON parsing: more flexible config

### Fixed
- Program crashing in some situations
- Clear report showing wrong error code
- Caches being cleared by paths, instead of by commands
- Not cleaning cache by commands properly
- Fixed config bugs: name/path dublicates not adding to list properly and more