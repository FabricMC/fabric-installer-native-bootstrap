// Require use of WinMain entrypoint so cmd doesn't flash open
// See RFC 1665 for additional details
#![windows_subsystem = "windows"]

mod bindings {
    windows::include_bindings!();
}

use bindings::{
    Windows::Win32::SystemServices::{CreateMutexA},
    Windows::Win32::WindowsProgramming::{CloseHandle},
    Windows::Win32::Debug::{GetLastError, WIN32_ERROR},
};

use winreg::RegKey;
use std::process::{Command, exit};
use std::io::Result;
use std::env;
use native_dialog::MessageDialog;
use native_dialog::MessageType;
use webbrowser;
use std::path::{PathBuf, Path};
use std::ffi::OsString;

fn main() {
    if let Ok(dir) = get_minecraft_installation_dir() {
        println!("Minecraft install dir {}", dir.display());
        try_minecraft_java(&dir);
    } else {
        println!("Could not find minecraft install dir.");
    }

    if let Some(val) = env::var_os("JAVA_HOME") {
        let mut path = PathBuf::from(val);
        path.push("bin/javaw.exe");
        launch_if_valid_java_installation(&path)
    }

    // Getting thin on the ground, lets check the path.
    launch_if_valid_java_installation("javaw");

    show_error();
}

fn try_minecraft_java<P: AsRef<Path>>(dir: P) -> bool {
    let dir = dir.as_ref();

    let paths = [
        "runtime/jre-legacy/windows-x64/jre-legacy/bin/javaw.exe",
        "runtime/jre-legacy/windows-x86/jre-legacy/bin/javaw.exe",
        "runtime/jre-x64/bin/javaw.exe",
        "runtime/jre-x86/bin/javaw.exe"
    ];

    for path in &paths {
        let full_path = dir.join(path);
        launch_if_valid_java_installation(full_path)
    }

    // None of the above paths were valid
    false
}

fn get_minecraft_installation_dir() -> Result<PathBuf> {
    let hcu = RegKey::predef(winreg::enums::HKEY_CURRENT_USER);
    let launcher = hcu.open_subkey(r"SOFTWARE\Mojang\InstalledProducts\Minecraft Launcher")?;
    let install_location: Result<OsString> = launcher.get_value("InstallLocation");
    install_location.map(|s| PathBuf::from(s))
}

fn launch_if_valid_java_installation<P: AsRef<Path>>(path: P) {
    let path = path.as_ref();

    if !is_valid_java_installation(path) {
        return;
    }

    let launch_exe = env::current_exe().expect("could not get path to current executable");

    let launcher_open = is_mojang_launcher_mutex_open();
    println!("Mojang Launcher open: {}", launcher_open);

    let status = Command::new(path)
        .arg(format!("-Dfabric.installer.mojanglauncher.open={}", launcher_open))
        .arg("-jar")
        .arg(launch_exe)
        .args(env::args())// Pass the run args through
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
        .set_text("The Fabric Installer could not find a valid Java installation.\n\nWould you like to open the Fabric wiki to find out how to fix this?\n\nURL: https://fabricmc.net/wiki/player:tutorials:java:windows")
        .show_confirm()
        .expect("Failed to show dialog");

    if open {
        webbrowser::open("https://fabricmc.net/wiki/player:tutorials:java:windows")
            .expect("Failed to open browser");
    }

    // Graceful exit otherwise windows may show additional help
    exit(0)
}

fn is_mojang_launcher_mutex_open() -> bool {
    unsafe {
        let mutex_handle = CreateMutexA(std::ptr::null_mut(), true, "MojangLauncher");

        if GetLastError() == WIN32_ERROR::ERROR_ALREADY_EXISTS {
            // Already exists so must be already open.
            return true;
        }

        // Dont worry its safe ;)
        CloseHandle(mutex_handle);
    }

    return false;
}