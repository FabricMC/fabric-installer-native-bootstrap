extern crate embed_resource;

fn main() {
    // Include our program resources for visual styling and DPI awareness on windows
    if cfg!(windows) {
        embed_resource::compile("platform/windows/program.rc")
    }
}
