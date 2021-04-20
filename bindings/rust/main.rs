#[link(name = "r30sponge")]
extern {
    fn r30s_new_default() -> *mut u8;
    fn r30s_absorb_byte(sponge: *mut u8, byte: u8);
    fn r30s_squeeze_byte(spong: *mut u8) -> u8;
}

fn main() {
    unsafe {
        let sponge = r30s_new_default();

        for c in "Test message".bytes() {
            r30s_absorb_byte(sponge, c);
        }

        for _ in 0..5 {
            println!("Lucky number: {}", r30s_squeeze_byte(sponge));
        }
    }
}
