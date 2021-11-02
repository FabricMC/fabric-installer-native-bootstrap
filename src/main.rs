// Require use of WinMain entrypoint so cmd doesn't flash open
// See RFC 1665 for additional details
#![windows_subsystem = "windows"]

use std::process::{Command, exit};
use std::env;
use std::ffi::c_void;
use std::io::Error;
use std::path::{PathBuf, Path};
use std::ptr::null_mut;
use windows::{
    Win32::{
        UI::{
            Shell::ShellExecuteW,
            WindowsAndMessaging::{
                MessageBoxW,
                SW_SHOW, MB_ICONINFORMATION, MB_YESNO, IDYES,
            },
        },
        System::Registry::{
            RegGetValueW,
            HKEY,
            HKEY_CURRENT_USER, RRF_RT_REG_SZ,
        },
        Foundation::{
            ERROR_SUCCESS
        },
    }
};

const MB_TILE: &str = "Fabric Installer";
const MB_BODY: &str = "The Fabric Installer could not find a valid Java installation.\n\nWould you like to open the Fabric wiki to find out how to fix this?\n\nURL: https://fabricmc.net/wiki/player:tutorials:java:windows";

const REG_HIVE: HKEY = HKEY_CURRENT_USER;
const REG_PATH: &str = r"SOFTWARE\Mojang\InstalledProducts\Minecraft Launcher";
const REG_KEY: &str = "InstallLocation";

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

fn get_minecraft_installation_dir() -> Result<PathBuf, Error> {
    let mut buffer: Vec<u16> = vec![0; 256];
    let mut data_size: u32 = buffer.capacity() as u32;
    let mut ret_code = unsafe {
        RegGetValueW(
            REG_HIVE,
            REG_PATH,
            REG_KEY,
            RRF_RT_REG_SZ,
            null_mut(),
            null_mut(),
            &mut data_size,
        )
    };

    if ret_code.0 != ERROR_SUCCESS.0 as i32 {
        return Err(Error::from_raw_os_error(ret_code.0));
    }

    buffer.resize((data_size / 2 - 1) as usize, 0);

    ret_code = unsafe {
        RegGetValueW(
            REG_HIVE,
            REG_PATH,
            REG_KEY,
            RRF_RT_REG_SZ,
            null_mut(),
            buffer.as_mut_ptr() as *mut c_void,
            &mut data_size,
        )
    };

    if ret_code.0 != ERROR_SUCCESS.0 as i32 {
        return Err(Error::from_raw_os_error(ret_code.0));
    }

    // Remove \0
    buffer.resize(buffer.len() - 1, 0);

    let data = String::from_utf16(&buffer).expect("could not convert data");

    return Ok(PathBuf::from(data));
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
            }
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
    let result = unsafe {
        MessageBoxW(
            None,
            MB_BODY,
            MB_TILE,
            MB_ICONINFORMATION | MB_YESNO,
        )
    };

    if result == IDYES {
        unsafe {
            ShellExecuteW(
                None,
                None,
                "https://fabricmc.net/wiki/player:tutorials:java:windows",
                None,
                None,
                SW_SHOW.0 as i32,
            );
        };
    }

    // Graceful exit otherwise windows may show additional help
    exit(0)
}