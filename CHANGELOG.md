# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased
[Link to Github compare]

## [0.12.7] - 2025-06-17
This release simplifies the use of JSON strings for the user.
### Added
- `json_parse` has been added to the public C API.
### Changed
- Users can now pass string, bool strings, number strings and JSON strings directly into `webview_return` without the need to double escape them.
### Security
- Sanitising checks for input strings is managed in the frontend JS using regex and a sandboxed iframe. Executable code, HTML scripts and URLEncoded HTML scripts are rejected. 

## [0.12.6] - 2025-06-17
This is a bugfix release that fixes inconsistent behaviour between OS platforms in a `bind(); init(); run();` or `bind(); set_html(); run();` call sequence.
Depending on platform, the bound script, init script or `<script>` tags are never processed, leading to silent failure.
### Fixed
- bound functions for init scripts and HTML scripts will execute consistentently across platforms.
### Changed
- If the user owns the window, Linux and Mac will call "set_html` after the main Webview script injection. This ensures that the DOM is ready to recieve scripts.
- If the user owns the window, Windows will reload the browser location after pre-dom script injection, ensuring that the scripts are triggered. 

## [0.12.5] - 2025-06-17
This release introduces optional Webview trace log output
### Added
- The `webview::log::trace` API with contextual call methods.
- Key API code flow locations are traced.
- The `WEBVIEW_LOG_TRACE` bool compiler option (defaults to OFF).
### Changed
- CI tests have `WEBVIEW_LOG_TRACE` set to ON, so failed tests will now verbosely output a trace log.

## [0.12.4] - 2025-06-17
This release introduces optional Webview error and warning log output. This should enhance the user and developer experience. 
### Added
- The `webview::log::console` API with `error`, `warn` and `info` methods.
- The `error` and `warn` methods have been implemeted in key API locations and integrated with the existing Webview error system.
- The `WEBVIEW_LOG` bool compiler option (defaults to OFF).
- The `WEBVIEW_LOG_ANSI` bool compiler option to toggle ANSI colourised strings (defaults to ON).
- Windows does not automatically capture `stdout` and `stderr` for GUI apps, so Webview will automatically check if the user has already captured these and capture them if not.
### Changed
- CI tests have `WEBVIEW_LOG` set to ON, so test failures will now verbosely output warnings and errors.
- Windows CI tests are now run as a GUI app.

## [0.12.3] - 2025-06-16
This release guarantees thread safety, thus simplifying the public API and providing the user a cleaner approach to building Webview based applications.
### Changed
- Webview automatically detects if the thread context is `main`, and internally re-directs API calls to `dispatch_impl` appropriately.
- Webview class initialisation MUST happen on the `main` thread, and Webview will now throw an exception if not.
- `run` and `init` SHOULD be called on the `main` thread, and Webview will now return an error if not.
- The Webview headers are now built with `Threads::Threads` as a dependency.
- Windows CI tests have `pthread` statically linked, else MSVC builds will fail to resolve threading library paths.
- Windows CI warmup test uses std::thread to check linkages.
### Deprecated
- `webview_dispatch` is no longer needed due to guaranteed thread safety. The user can make API calls freely from any context.
- `webview_destroy`. The legacy implementation of this API function was in violation of RAII, and likely to cause undefined behaviour. From a user perspective, it is ambiguous and easily confused with `webview_terminate`. It now safely re-directs itself to `webview_terminate` and is no-op if called after `webview_terminate`.

## [0.12.2] - 2025-06-16
This is primarily a housekeeping release aimed at code readability, maintainability and enabling work for upcoming enhancements.<br>
It has a large diff footprint, but does not change existing functionality ensuring 100% backwards compatibility.
### Changed
- The 'core/include/webview' folder structure is flattened to 'core/include'.
- The file folder structure and namespaces have been coordinated to reflect code purpose and encapsulation.
- Namespace tails are followed by API structs and classes with improved inline documentation about the purpose of each internal API.
- Much of the code definitions and implementations have been separated with improved inline documentation in header files.
- Some existing code has been refactored for improved encapsulation and readability.
- All JS/HTML string generation has been moved to a tokenised template system with a string retrieval API.
### Fixed
- Unlike Linux and Windows, the Darwin backend does not accept `navigate` URL strings with leading or trailing whitespace, causing silent execution anomalies. URL strings are now automatically trimmed for Darwin.
### Deprecated
- The `webview::webview` C++ API class naming is ambiguous for compilers and humans, so `webview_cc` is now declared in the Global namespace and should be used instead.
- `#include "webview/webview.h"` is deprecated in favour of `#include "webview.h"`. 

## Released


## [0.12.1] - 2025-06-16
For a while all development has occured directly on v0.12.0 HEAD.<br>
This is a benchmark release reflecting all bugfixes and optimisations since 2024-09-11

## [0.12.0] - 2024-09-11

### Added

- Use CMake `cxx_std_11` compile feature ([#1135](https://github.com/webview/webview/pull/1135))

### Fixed

- ODR issues caused by functions not being marked with `inline` ([#1138](https://github.com/webview/webview/pull/1138))

### Breaking Changes

- Synchronize CMake alias targets and export names ([#1140](https://github.com/webview/webview/pull/1140))

## [0.11.0] - 2024-08-29

### Added

- New compile-time options for controlling `WEBVIEW_API` ([#893](https://github.com/webview/webview/pull/893))
- Support for WebKitGTK API 4.1 ([#1022](https://github.com/webview/webview/pull/1022))
- Support for WebKitGTK API 6.0 and GTK 4 ([#1125](https://github.com/webview/webview/pull/1125))
- CMake build system ([#1130](https://github.com/webview/webview/pull/1130))

### Changed

- Disable status bar (Windows/WebView2) ([#1028](https://github.com/webview/webview/pull/1028))
- Reworked handling and reporting of errors ([#1099](https://github.com/webview/webview/pull/1099))

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

[Link to Github compare]: https://github.com/webview/webview/compare/0.12.7...HEAD
[0.12.7]:     https://github.com/webview/webview/compare/0.12.6...0.12.7
[0.12.6]:     https://github.com/webview/webview/compare/0.12.5...0.12.6
[0.12.5]:     https://github.com/webview/webview/compare/0.12.4...0.12.5
[0.12.4]:     https://github.com/webview/webview/compare/0.12.3...0.12.4
[0.12.3]:     https://github.com/webview/webview/compare/0.12.2...0.12.3
[0.12.2]:     https://github.com/webview/webview/compare/0.12.1...0.12.2
[0.12.1]:     https://github.com/webview/webview/compare/0.12.0...0.12.1
[0.12.0]:     https://github.com/webview/webview/compare/0.11.0...0.12.0
[0.11.0]:     https://github.com/webview/webview/compare/0.10.0...0.11.0
[0.10.0]:     https://github.com/webview/webview/compare/0.1.1...0.10.0
[0.1.1]:      https://github.com/webview/webview/compare/0.1.0...0.1.1
[0.1.0]:      https://github.com/webview/webview/releases/tag/0.1.0
