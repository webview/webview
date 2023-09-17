# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- New compile-time options for controlling `WEBVIEW_API` ([#893](https://github.com/webview/webview/pull/893))

### Changed

- Disable status bar (Windows/WebView2) ([#1028](https://github.com/webview/webview/pull/1028))

### Breaking Changes

- `WEBVIEW_API` is now `inline` by default for C++ ([#893](https://github.com/webview/webview/pull/893))
- Treat result of binding function as JSON, not JS ([#1002](https://github.com/webview/webview/pull/1002))
- App lifecycle separation ([#1005](https://github.com/webview/webview/pull/1005))

## [0.10.0] - 2023-09-16

This is the first release since the library rewrite by [zserge](https://github.com/zserge) ([#315](https://github.com/webview/webview/pull/315)), and is a necessary one that allows us to prepare for future changes in the library.

Due to the vast amount of contributions that are in this release on top of the changes and removals introduced in the library rewrite, we've picked a few contributions aside from the rewrite that had a significant impact compared to the previous release.

### Added

Windows:

- Microsoft Edge WebView2 backend ([#315](https://github.com/webview/webview/pull/315))
- Custom WebView2Loader implementation ([#783](https://github.com/webview/webview/pull/783))
- DPI scaling for Windows 10 and later ([#982](https://github.com/webview/webview/pull/982))
- Add support for dark title bar on Windows 10 and later ([#996](https://github.com/webview/webview/pull/996))

### Removed

- MSHTML backend ([#315](https://github.com/webview/webview/pull/315))
- Go binding ([#1009](https://github.com/webview/webview/pull/1009)) — moved to [webview/webview_go](https://github.com/webview/webview_go)

## [0.1.1] - 2020-01-21

## [0.1.0] - 2018-05-09

[unreleased]: https://github.com/webview/webview/compare/0.10.0...HEAD
[0.10.0]:     https://github.com/webview/webview/compare/0.1.1...0.10.0
[0.1.1]:      https://github.com/webview/webview/compare/0.1.0...0.1.1
[0.1.0]:      https://github.com/webview/webview/releases/tag/0.1.0
