const os = require("node:os");
if (os.platform() === "win32") {
  const fs = require("node:fs");
  const path = require("node:path");
  const { execSync } = require("node:child_process");
  const dest = path.join(process.cwd(), "nuget.exe");
  console.log(dest);

  if (!fs.existsSync(dest)) {
    const url = "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe";

    const command = `wget -O ${dest} ${url}`;
    execSync(command, { stdio: "inherit" });
  }
}
