const fs = require("node:fs");
const path = require("node:path");
const { execSync } = require("node:childProcess");
const dest = path.join(process.cwd(), "nuget.exe");

console.log(dest);

if (!fs.existsSync(dest)) {
  const url = "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe";

  const command = `wget -O ${dest} ${url}`;
  execSync(command, { stdio: "inherit" });
}
