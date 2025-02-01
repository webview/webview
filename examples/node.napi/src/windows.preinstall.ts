const os = require("node:os");
const fs = require("node:fs");
const path = require("node:path");
const { exec } = require("node:child_process");
const nugetDest = path.join(process.cwd(), "nuget.exe");
const MsWebView2Dir = path.join(process.cwd(), "src");
const nugetUrl = "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe";

if (os.platform() === "win32") {
  fetchNuget()
    .then(downloadMsWebView2)
    .catch((err) => {
      throw err;
    });
}

async function fetchNuget() {
  if (fs.existsSync(nugetDest)) return;
  try {
    const res = await fetch(nugetUrl);
    if (!res.ok)
      throw new Error(
        `HTTP error downloading nuget.exe! status: ${res.status}`
      );
    const buffer = await res.arrayBuffer();
    fs.writeFileSync(nugetDest, Buffer.from(buffer), { mode: 0o755 });
  } catch (err) {
    throw err;
  }
}
async function downloadMsWebView2() {
  const command = `nuget.exe install Microsoft.Web.WebView2 -OutputDirectory ${MsWebView2Dir}`;
  try {
    const cp = await exec(command, (err: Error) => {
      if (err) throw err;
    });
  } catch (err) {
    throw err;
  }
}
