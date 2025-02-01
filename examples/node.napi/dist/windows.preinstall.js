"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
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
function fetchNuget() {
    return __awaiter(this, void 0, void 0, function* () {
        if (fs.existsSync(nugetDest))
            return;
        console.info("Downloading nuget.exe");
        try {
            const res = yield fetch(nugetUrl);
            if (!res.ok)
                throw new Error(`HTTP error downloading nuget.exe! status: ${res.status}`);
            const buffer = yield res.arrayBuffer();
            fs.writeFileSync(nugetDest, Buffer.from(buffer), { mode: 0o755 });
        }
        catch (err) {
            throw err;
        }
    });
}
function downloadMsWebView2() {
    return __awaiter(this, void 0, void 0, function* () {
        const isDownloaded = fs
            .readdirSync(MsWebView2Dir)
            .find((name) => name.startsWith("Microsoft.Web.WebView2"));
        if (!!isDownloaded)
            return;
        console.info("Downloading Microsoft.Web.WebView2 with nuget.exe");
        const command = `nuget.exe install Microsoft.Web.WebView2 -OutputDirectory ${MsWebView2Dir}`;
        try {
            const cp = yield exec(command, (err) => {
                if (err)
                    throw err;
            });
        }
        catch (err) {
            throw err;
        }
    });
}
