// Require use of WinMain entrypoint so cmd doesn't flash open
// See RFC 1665 for additional details
#![windows_subsystem = "windows"]

use winreg::RegKey;
use std::process::{Command, exit};
use std::io::Result;
use std::env;
use native_dialog::MessageDialog;
use native_dialog::MessageType;
use webbrowser;
use std::path::{PathBuf, Path};
use path_slash::PathBufExt;

fn main() {
    if let Ok(dir) = get_minecraft_installation_dir() {
        println!("Minecraft install dir {}", dir.display());
        try_minecraft_java(&dir);
    } else {
        println!("Could not find minecraft install dir.");
    }

    // if let Some(val) = env::var_os("JAVA_HOME") {
    //     let mut path = PathBuf::from(val);
    //     path.push("bin/javaw.exe");
    //     launch_if_valid_java_installation(&path)
    // }

    // // Getting thin on the ground, lets check the path.
    // launch_if_valid_java_installation("javaw");

    show_error();
}

fn try_minecraft_java<P: AsRef<Path>>(dir: P) -> bool {
    let dir = dir.as_ref();

    let paths = [
        r"runtime\java-runtime-alpha\windows-x64\java-runtime-alpha\bin\javaw.exe",
        r"runtime\java-runtime-alpha\windows-x86\java-runtime-alpha\bin\javaw.exe",
        r"runtime\jre-legacy\windows-x64\jre-legacy\bin\javaw.exe",
        r"runtime\jre-legacy\windows-x86\jre-legacy\bin\javaw.exe",
        r"runtime\jre-x64\bin\javaw.exe",
        r"runtime\jre-x86\bin\javaw.exe"
    ];

    for path in &paths {
        let full_path = dir.join(path);
        if full_path.exists() {
            launch_if_valid_java_installation(full_path)
        }
    }

    // None of the above paths were valid
    false
}

fn get_minecraft_installation_dir() -> Result<PathBuf> {
    let hcu = RegKey::predef(winreg::enums::HKEY_CURRENT_USER);
    let launcher = hcu.open_subkey(r"SOFTWARE\Mojang\InstalledProducts\Minecraft Launcher")?;
    let install_location: Result<String> = launcher.get_value("InstallLocation");
    return install_location.map(|s| PathBuf::from_slash(s))
}

fn launch_if_valid_java_installation<P: AsRef<Path>>(path: P) {
    let path = path.as_ref();

    if !is_valid_java_installation(path) {
        return;
    }

    let launch_exe = env::current_exe().expect("could not get path to current executable");

    let status = Command::new(path)
        .arg("-jar")
        .arg(launch_exe)
        .status();

    if let Ok(status) = status {
        match status.code() {
            Some(code) => exit(code),
            None => {
                unimplemented!("No signal handling implemented")
            },
        }
    }

    println!("Unknown exit code from jvm installer.");
    exit(-1);
}

fn is_valid_java_installation<P: AsRef<Path>>(path: P) -> bool {
    let path = path.as_ref();

    let status = Command::new(path)
        .arg("-version")
        .status();

    if let Ok(status) = status {
        println!("Found java java installation at: {}", path.display());
        status.success()
    } else {
        println!("No valid java installation found at: {}", path.display());
        false
    }
}

fn show_error() -> ! {
    let open = MessageDialog::new()
        .set_type(MessageType::Error)
        .set_title("Fabric Installer")
        .set_text("The Fabric Installer could not find a valid Java installation installed by Minecraft.\n\nPlease install and launch minecraft, or try the universal '.jar' fabric installer.\n\nWould you like to open the Fabric wiki for more help?\n\nURL: https://fabricmc.net/wiki/player:tutorials:java:windows")
        .show_confirm()
        .expect("Failed to show dialog");

    if open {
        webbrowser::open("https://fabricmc.net/wiki/player:tutorials:java:windows")
            .expect("Failed to open browser");
    }

    // Graceful exit otherwise windows may show additional help
    exit(0)
}