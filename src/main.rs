extern "C" {
    fn processor_init() -> *mut Processor;
    fn processor_address_from_mode(
        processor: *mut Processor,
        addr_mode: u8,
        addr_out: *mut u16,
        boundary_crossed_out: *mut bool,
    );
    fn processor_free(processor: *mut Processor);
    fn processor_tick(processor: *mut Processor);
    fn processor_memory_read(processor: *const Processor, addr: u16) -> u8;
    fn processor_memory_write(processor: *mut Processor, addr: u16, value: u8);
}

#[repr(C)]
#[derive(Debug)]
struct Processor {
    stack_pointer: u8,
    program_counter: u16,
    accumulator: u8,
    index_x: u8,
    index_y: u8,
    status: u8,
    remaining_cycles: u8,
    memory_cpu: [u8; 2000],
}

impl Processor {
    pub fn new() -> Self {
        unsafe {
            let p = processor_init();
            let p = std::ptr::read(p);
            p
        }
    }

    pub fn read(&self) -> u8 {
        let processor = self as *const Self;
        unsafe { processor_memory_read(processor, 0x1FFF) }
    }

    pub fn write(&mut self, addr: u16) {
        let processor = self as *mut Self;
        unsafe { processor_memory_write(processor, addr, value) };
    }
}

fn main() {
    let processor = Processor::new();
    let value = processor.read();
    dbg!(value);
    // unsafe {
    //     let processor = processor_init();
    //     processor_tick(processor);
    // }
}
