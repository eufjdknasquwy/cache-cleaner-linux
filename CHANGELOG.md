# Changelog

## Latest ([v0.4.0])

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

### Changed
- Improved UI: cache sorted in tables, custom style with animations
- Fixed crashes on incorrect JSON config
- Improved JSON parsing: more flexible config

### Fixed
- Program crashing in some situations
- Clear report showing wrong error code
- Caches being cleared by paths, instead of by commands
- Not cleaning cache by commands properly
- Fixed config bugs: name/path dublicates not adding to list properly and more