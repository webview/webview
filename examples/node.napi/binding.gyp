# pylint: disable=missing-module-docstring
# pylint: disable=pointless-statement
{

    'targets': [
        {
            'target_name': 'webview.napi',
            'sources': ['src/webview.napi_wrap.cxx'],
            'include_dirs': [
                "<!@(node -p \"require('node-addon-api').include\")",
                "../../core/include/webview",
                "./src"
            ],
            'dependencies': ["<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except_all"],

            'conditions': [

                ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"',
                 {
                     'include_dirs': [
                         " <!(pkg-config --cflags gtk4 webkitgtk-6.0)"
                     ],
                     'libraries': [" <!(pkg-config --libs gtk4 webkitgtk-6.0)", "-ldl"],
                     'cflags': ["-std=c++17", "-O2"],
                 }
                 ],
                ['OS=="mac"',
                 {
                     'xcode_settings': {
                         'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
                         "OTHER_LDFLAGS": ["-framework", "WebKit", "-ldl"],
                     },
                     "cflags_cc": ["-std=c++17", "-O2"]
                 }
                 ],
                ['OS=="win"', {
                    'msbuild_settings': {
                        'ClCompile': {
                            'LanguageStandard': 'stdcpp17',
                            'DisableSpecificWarnings': ['4005']
                        }
                    },
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            "Optimization": 2,
                            "BufferSecurityCheck": "true",
                            "RuntimeLibrary": 0,
                        },
                        "VCLinkerTool": {
                            "AdditionalDependencies": [
                                "bufferoverflowU.lib",
                                "ntdll.lib"
                            ],
                            "AdditionalOptions": [
                                "/DYNAMICBASE"
                                "/NODEFAULTLIB:LIBCMT",
                                "/DELAYLOAD:webview2loader.dll"
                            ]
                        }
                    },

                    'variables': {
                        'WV2_VERSION%': '<!(python ./src/get_mswv2_version.py)'
                    },
                    'include_dirs': [
                        "./src/Microsoft.Web.WebView2.<(WV2_VERSION)/build/native/include"
                    ],
                    "libraries": [
                        "AdvAPI32.lib",
                        "Ole32.lib",
                        "shell32.lib",
                        "ShLwApi.lib",
                        "User32.lib",
                        "Version.lib"
                    ]
                }]
            ]
        },
        {
            'target_name': 'jscallback',
            'defines': ['V8_DEPRECATION_WARNINGS=1'],
            'sources': ['src/JsCallback.cc'],
            'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", "./src"],
            'dependencies': ["<!(node -p \"require('node-addon-api').targets\"):node_addon_api_except_all"],
            'conditions': [
                ['OS=="linux" or OS=="freebsd" or OS=="openbsd" or OS=="solaris"',
                 {
                     'cflags': ["-std=c++17", "-O2"],
                 }
                 ],
                ['OS=="mac"', {
                    'xcode_settings': {
                        'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
                    },
                    'cflags': ["-std=c++17", "-O2"],
                }],
                ['OS=="win"', {
                    'msbuild_settings': {
                        'ClCompile': {
                            'LanguageStandard': 'stdcpp17',
                        }
                    },
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            "Optimization": 2,
                            "RuntimeLibrary": 0,
                        },
                    },
                }]
            ],

        }

    ]
}
