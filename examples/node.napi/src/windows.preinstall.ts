const os = require("node:os");
const fs = require("node:fs");
const path = require("node:path");
const { exec } = require("node:child_process");
const nugetDest = path.join(process.cwd(), "nuget.exe");
const srcDir = path.join(process.cwd(), "src");
const nugetUrl = "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe";
const MsWv2Tag = "Microsoft.Web.WebView2";
const MsWv2Version = "1.0.2957.106";

if (os.platform() === "win32") {
  setMsWv2Version()
    .then(fetchNuget)
    .then(downloadMsWebView2)
    .catch((err) => {
      throw err;
    });
}

async function setMsWv2Version() {
  const command = `npm config set wv2_version ${MsWv2Version}`;
  try {
    await exec(command, (err: Error) => {
      if (err) throw err;
    });
  } catch (err) {
    throw err;
  }
}
async function fetchNuget() {
  if (fs.existsSync(nugetDest)) return;
  console.info("Downloading nuget.exe");
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
  const isDownloaded = fs.existsSync(
    path.join(srcDir, `${MsWv2Tag}.${MsWv2Version}`)
  );
  if (!!isDownloaded) return;

  console.info("Downloading Microsoft.Web.WebView2 with nuget.exe");
  const command = `nuget.exe install ${MsWv2Tag} -Version ${MsWv2Version} -OutputDirectory ${srcDir}`;
  try {
    await exec(command, (err: Error) => {
      if (err) throw err;
    });
    fs.writeFileSync(
      path.join(process.cwd(), "MsWv2Version"),
      JSON.stringify({ MsWv2Version })
    );
  } catch (err) {
    throw err;
  }
}
