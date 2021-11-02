fn main() {
    // Include our program resources for visual styling and DPI awareness on windows
    if cfg!(windows) {
        let result = winres::WindowsResource::new()
            .set_icon("res/icon.ico")
            .set("ProductName", "Fabric Installer")
            .set("CompanyName", "Fabric Team")
            .set("LegalCopyright", "Apache License Version 2.0")
            .set_manifest_file("platform/windows/program.manifest")
            .compile();

        if let Err(_) = result {
            panic!("Failed to set windows resources");
        }
    }
}
