package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
)

func fetch_mswebview2(libs_dir string, set_env bool, version string) {
	lib_dir := filepath.Join(libs_dir, "webview2-" + version)
	_, err := os.Stat(filepath.Join(lib_dir, "Microsoft.Web.WebView2.nuspec"))
	if err != nil || version == "latest" {
		if err == nil {
			err = os.RemoveAll(lib_dir)
			if err != nil {
				log.Fatal(err)
			}
		}
		fmt.Printf("Fetching WebView2 %s...\n", version)
		os.MkdirAll(lib_dir, os.ModePerm) // TODO: reduce permissions
		url := "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/"
		if version != "latest" {
			url += version
		}
		curl_cmd := exec.Command("curl", "-sSL", url)
		curl_stdout, err := curl_cmd.StdoutPipe()
		if err != nil {
			log.Fatal(err)
		}
		defer curl_stdout.Close()
		tar_cmd := exec.Command("tar", "-xf", "-", "-C", lib_dir)
		tar_cmd.Stdin = curl_stdout
		curl_cmd.Start()
		_, err = tar_cmd.Output()
		if err != nil {
			log.Fatal(err)
		}
	}
	if set_env {
		get_env_cmd := exec.Command("go", "env", "-json")
		env_output, err := get_env_cmd.Output()
		if err != nil {
			log.Fatal(err)
		}
		env := map[string]string{}
		err = json.Unmarshal(env_output, &env)
		if err != nil {
			log.Fatal(err)
		}
		include_subdir := filepath.Join("build", "native", "include")
		include_dir := filepath.Join(lib_dir, include_subdir)
		if strings.Contains(env["CGO_CXXFLAGS"], include_dir) {
			return
		}
		fmt.Printf("Updating the environment for WebView2 %s...\n", version)
		cxxflags := fmt.Sprintf("CGO_CXXFLAGS=%s \"-I%s\"", env["CGO_CXXFLAGS"], include_dir)
		set_env_cmd := exec.Command("go", "env", "-w", cxxflags)
		err = set_env_cmd.Run()
		if err != nil {
			log.Fatal(err)
		}
	}
}

func cmd_fetch_libs(cmd string, args []string) {
	var libs_dir string
	var set_env bool
	var webviw2_version string
	flags := flag.NewFlagSet(cmd, flag.ExitOnError)
	flags.StringVar(&libs_dir, "libs-dir", "libs", "Libraries output directory.")
	flags.BoolVar(&set_env, "set-env", false, "Set environment variables for go.")
	flags.StringVar(&webviw2_version, "webview2-version", "1.0.1150.38", "WebView2 version to use or \"latest\".")
	flags.Parse(args)

	if !filepath.IsAbs(libs_dir) {
		wd, err := os.Getwd()
		if err != nil {
			log.Fatal(err)
		}
		libs_dir = filepath.Join(wd, libs_dir)
	}
	os := runtime.GOOS
	if os == "windows" {
		fetch_mswebview2(libs_dir, set_env, webviw2_version)
	}
}

func main() {
	commands := map[string]func(string, []string){"fetch-libs": cmd_fetch_libs}
	args := os.Args[1:]
	if len(args) == 0 {
		fmt.Println("Commands:")
		for k, _ := range commands {
			fmt.Printf("  %s\n", k)
		}
		os.Exit(1)
	}
	cmd := args[0]
	args = args[1:]
	for k, v := range commands {
		if (k == cmd) {
			v(cmd, args)
			return
		}
	}
	fmt.Printf("Invalid command: %s\n", cmd)
	os.Exit(1)
}
