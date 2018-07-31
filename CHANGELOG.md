<a name=""></a>
##  1.6.3 (2018-07-31)




# Change Log
Simplest color picker.

## [Unreleased]

## [1.6.2] - 2018-1-31
- Update translation
- Regenerate screenPixmap when the color is picked, thanks Bet4
- Provide dbus API to make other application can call deepin-picker for color pick

## [1.6.1] - 2017-11-22
- Update translation

## [1.5] - 2017-11-17
### Features
- Install dbus service

### Bug Fixes
- Flatpak_icon_workaround
- Set color_type default value

## [1.5] - 2017-11-06
- Support FlatPak, add dbus service file
- Fix color type can't save in config file with FlatPak environment
- Synchronous translations

## [1.4] - 2017-11-02
- Fix mouse event pass to application window under picker

## [1.3] - 2017-11-02
### Added
- Use DBus start process to compatible with FlatPak
- Use DBus popup composite warning dialog to compatible with FlatPak

### Fixed
- Fix left button got empty color.

## [1.2] - 2017-11-02
### Added
- Support CMYK color
- Support HSV color
- Support HiDPI
- Popup warning dialog if composite is not enable in system
- Update translations from transifex.com

### Fixed
- Fix the edge of the magnifying glass
- Fix color format restore to default after restart
- Fix the first system notification can not be hidden immediately


