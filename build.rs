extern crate embed_resource;

fn main() {
    // Include our program resources for visual styling and DPI awareness on windows
    if cfg!(windows) {
        embed_resource::compile("platform/windows/program.rc");

        let result = winres::WindowsResource::new()
            .set_icon("res/icon.ico")
            .set("ProductName", "Fabric Installer")
            .set("CompanyName", "Fabric Team")
            .set("LegalCopyright", "Apache License Version 2.0")
            .compile();

        if let Err(_) = result {
            panic!("Failed to set windows resources");
        }
    }
}
