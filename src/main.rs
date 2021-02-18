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

fn main() {
    if let Ok(dir) = get_minecraft_installation_dir() {
        println!("Minecraft install dir {}", dir);
        try_minecraft_java(&dir);
    } else {
        println!("Could not find minecraft install dir.");
    }

    match env::var("JAVA_HOME") {
        Ok(val) => {
            let full_path: String = val.to_owned() + r"bin\javaw.exe";
            launch_if_valid_java_installation(&full_path)
        },
        Err(_) => {},
    }

    // Getting thin on the ground, lets check the path.
    launch_if_valid_java_installation(&"javaw".to_string());

    show_error();
}

fn try_minecraft_java(dir: &String) -> bool {
    let paths: [&str; 4] = [
        r"runtime\jre-legacy\windows-x64\jre-legacy\bin\javaw.exe",
        r"runtime\jre-legacy\windows-x68\jre-legacy\bin\javaw.exe",
        r"runtime\jre-x64\bin\javaw.exe",
        r"runtime\jre-x86\bin\javaw.exe"
    ];

    for path in &paths {
        let full_path: String = dir.to_owned() + path;
        launch_if_valid_java_installation(&full_path)
    }

    // None of the above paths were valid
    return false;
}

fn get_minecraft_installation_dir() -> Result<String> {
    let hcu = RegKey::predef(winreg::enums::HKEY_CURRENT_USER);
    let launcher = hcu.open_subkey(r"SOFTWARE\Mojang\InstalledProducts\Minecraft Launcher")?;
    return launcher.get_value("InstallLocation");
}

fn launch_if_valid_java_installation(path: &String) {
    if !is_valid_java_installation(path) {
        return;
    }

    let args: Vec<String> = env::args().collect();
    let launch_exe: &String = &args[0];

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

fn is_valid_java_installation(path: &String) -> bool {
    let status = Command::new(path)
        .arg("-version")
        .status();

    if let Ok(status) = status {
        println!("Found java java installation at: {}", path);
        return status.success()
    }

    println!("No valid java installation found at: {}", path);
    return false;
}

fn show_error() -> ! {
    let result = MessageDialog::new()
        .set_type(MessageType::Error)
        .set_title("Fabric Installer")
        .set_text("Unable to find a valid Java Installation.\n\nWould you like to open a browser to the following page?\n\nhttps://fabricmc.net/wiki/player:tutorials:java:windows")
        .show_confirm();

    if let Err(_) = result {
        panic!("Failed to show dialog");
    }

    match result {
        Ok(open) => {
            if open {
                if !webbrowser::open("https://fabricmc.net/wiki/player:tutorials:java:windows").is_ok() {
                    panic!("Failed to open browser");
                }
            }
        },
        Err(_) => {
            panic!("Failed to show dialog");
        },
    }

    // Graceful exit otherwise windows may show additional help
    exit(0)
}