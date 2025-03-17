"""
It fetches the active MS WebView2 Nuget version from file.
"""

with open(".mswv2.nuget.v", "r", encoding='utf8') as file:
    version = file.read().strip()
    print(version)
