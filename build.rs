fn main() {
    println!("cargo:rerun-if-changed=src/processor/processor.c");

    cc::Build::new()
        .file("src/processor/processor.c")
        .compile("processor");
}
