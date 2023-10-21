trait Mapper {
    const ID: u8;

    fn read(&mut self, addr: u16) -> u8;
    fn observe_read(&mut self, addr: u16, value: u8);
    fn observe_write(&mut self, addr: u16, value: u8);
}
