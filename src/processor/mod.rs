mod status;

use std::fmt::{self, Debug};

use status::Status;

pub struct Processor {
    stackpointer: u8,
    program_counter: u16,
    accumulator: u8,
    index_x: u8,
    index_y: u8,
    status: Status,
    remaining_cycles: u8,
    memory: [u8; 0x10000],
}

impl Processor {
    pub fn new() -> Box<Self> {
        Box::new(Self {
            stackpointer: 0xFF,
            program_counter: 0x0000,
            accumulator: 0x00,
            index_x: 0x00,
            index_y: 0x00,
            status: Status::init(),
            remaining_cycles: 0,
            memory: [0; 0x10000],
        })
    }

    pub fn read(&self, addr: u16) -> u8 {
        self.memory[addr as usize]
    }

    pub fn write(&mut self, addr: u16, value: u8) {
        self.memory[addr as usize] = value;
    }

    pub fn accumulator(&self) -> u8 {
        self.accumulator
    }

    pub fn index_x(&self) -> u8 {
        self.index_x
    }

    pub fn index_y(&self) -> u8 {
        self.index_y
    }

    pub fn stack_top(&self) -> u8 {
        self.read(u16::from(self.stackpointer).wrapping_add(0x100))
    }

    pub fn stack_push(&mut self, value: u8) {
        self.write(u16::from(self.stackpointer).wrapping_add(0x100), value);
        self.stackpointer = self.stackpointer.wrapping_sub(1);
    }

    pub fn stack_pop(&mut self) -> u8 {
        let value = self.read(u16::from(self.stackpointer).wrapping_add(0x100));

        self.stackpointer = self.stackpointer.wrapping_add(1);

        value
    }

    pub fn stackpointer(&self) -> u16 {
        u16::from(self.stackpointer).wrapping_add(0x100)
    }

    pub fn program_counter(&self) -> u16 {
        self.program_counter
    }

    pub fn tick(&mut self) {
        let code = self.read(self.program_counter);

        if self.remaining_cycles > 0 {
            self.remaining_cycles -= 1;
            return;
        }

        self.execute_opcode(code);
    }

    pub fn tick_times(amount: u8) {}

    fn execute_opcode(&mut self, code: u8) {
        match code {
            // LDA
            0xA9 => self.opcode_ld(Register::Accumulator, AddressMode::Immediate),
            0xA5 => self.opcode_ld(Register::Accumulator, AddressMode::ZeroPage),
            0xB5 => self.opcode_ld(Register::Accumulator, AddressMode::ZeroPageX),
            0xAD => self.opcode_ld(Register::Accumulator, AddressMode::Absolute),
            0xBD => self.opcode_ld(Register::Accumulator, AddressMode::AbsoluteX),
            0xB9 => self.opcode_ld(Register::Accumulator, AddressMode::AbsoluteY),
            0xA1 => self.opcode_ld(Register::Accumulator, AddressMode::IndirectX),
            0xB1 => self.opcode_ld(Register::Accumulator, AddressMode::IndirectY),

            // LDX
            0xA2 => self.opcode_ld(Register::X, AddressMode::Immediate),
            0xA6 => self.opcode_ld(Register::X, AddressMode::ZeroPage),
            0xB6 => self.opcode_ld(Register::X, AddressMode::ZeroPageX),
            0xAE => self.opcode_ld(Register::X, AddressMode::Absolute),
            0xBE => self.opcode_ld(Register::X, AddressMode::AbsoluteX),

            // LDY
            0xA0 => self.opcode_ld(Register::Y, AddressMode::Immediate),
            0xA4 => self.opcode_ld(Register::Y, AddressMode::ZeroPage),
            0xB4 => self.opcode_ld(Register::Y, AddressMode::ZeroPageX),
            0xAC => self.opcode_ld(Register::Y, AddressMode::Absolute),
            0xBC => self.opcode_ld(Register::Y, AddressMode::AbsoluteX),

            // STA
            0x85 => self.opcode_st(Register::Accumulator, AddressMode::ZeroPage),
            0x95 => self.opcode_st(Register::Accumulator, AddressMode::ZeroPageX),
            0x8D => self.opcode_st(Register::Accumulator, AddressMode::Absolute),
            0x9D => self.opcode_st(Register::Accumulator, AddressMode::AbsoluteX),
            0x99 => self.opcode_st(Register::Accumulator, AddressMode::AbsoluteY),
            0x81 => self.opcode_st(Register::Accumulator, AddressMode::IndirectX),
            0x91 => self.opcode_st(Register::Accumulator, AddressMode::IndirectY),

            // STX
            0x86 => self.opcode_st(Register::X, AddressMode::ZeroPage),
            0x96 => self.opcode_st(Register::X, AddressMode::ZeroPageY),
            0x8E => self.opcode_st(Register::X, AddressMode::Absolute),

            // STY
            0x84 => self.opcode_st(Register::Y, AddressMode::ZeroPage),
            0x94 => self.opcode_st(Register::Y, AddressMode::ZeroPageY),
            0x8C => self.opcode_st(Register::Y, AddressMode::Absolute),

            // Transfer
            0xAA => self.opcode_transfer(Register::Accumulator, Register::X),
            0xA8 => self.opcode_transfer(Register::Accumulator, Register::Y),
            0xBA => self.opcode_transfer(Register::Stack, Register::X),
            0x8A => self.opcode_transfer(Register::X, Register::Accumulator),
            0x9A => self.opcode_transfer(Register::X, Register::Stack),
            0x98 => self.opcode_transfer(Register::Y, Register::Accumulator),

            // Decrement
            0xC6 => self.opcode_dec_address(AddressMode::ZeroPage),
            0xD6 => self.opcode_dec_address(AddressMode::ZeroPageX),
            0xCE => self.opcode_dec_address(AddressMode::Absolute),
            0xDE => self.opcode_dec_address(AddressMode::AbsoluteX),
            0xCA => self.opcode_dec_register(Register::X),
            0x88 => self.opcode_dec_register(Register::Y),

            // Increment
            0xE6 => self.opcode_inc_address(AddressMode::ZeroPage),
            0xF6 => self.opcode_inc_address(AddressMode::ZeroPageX),
            0xEE => self.opcode_inc_address(AddressMode::Absolute),
            0xFE => self.opcode_inc_address(AddressMode::AbsoluteX),
            0xE8 => self.opcode_inc_register(Register::X),
            0xC8 => self.opcode_inc_register(Register::Y),

            // Jump
            0x4C => self.opcode_jmp(AddressMode::Absolute),
            0x6C => self.opcode_jmp(AddressMode::Indirect),

            // Function
            0x20 => todo!(),
            0x60 => todo!(),

            // Interrupt
            0x40 => todo!(),

            // Branch

            // Stack
            0x48 => self.opcode_push_accumulator(),
            0x08 => todo!(),
            0x68 => self.opcode_pull_accumulator(),
            0x28 => todo!(),

            // NOP
            0xEA => {
                self.program_counter = self.program_counter.wrapping_add(1);
                self.remaining_cycles = 2;
            }
            _ => todo!(),
        }
    }

    fn opcode_ld(&mut self, register: Register, mode: AddressMode) {
        let (addr, boundary_status) = self.instruction_address_from_mode(mode);
        let value = self.read(addr);

        let register = match register {
            Register::Accumulator => &mut self.accumulator,
            Register::X => &mut self.index_x,
            Register::Y => &mut self.index_y,
            Register::Stack => panic!("Invalid register"),
        };

        *register = value;
        self.status.update_with_value(value);

        let (jump, cycles) = match mode {
            AddressMode::Immediate => (2, 2),
            AddressMode::ZeroPage => (2, 3),
            AddressMode::ZeroPageX => (2, 4),
            AddressMode::ZeroPageY => (2, 4),
            AddressMode::Absolute => (3, 4),
            AddressMode::AbsoluteX | AddressMode::AbsoluteY => (
                3,
                match boundary_status {
                    BoundaryStatus::Crossed => 5,
                    BoundaryStatus::Uncrossed => 4,
                },
            ),
            AddressMode::IndirectX => (2, 6),
            AddressMode::IndirectY => (
                2,
                match boundary_status {
                    BoundaryStatus::Uncrossed => 5,
                    BoundaryStatus::Crossed => 6,
                },
            ),
            _ => panic!("Invalid address mode"),
        };

        self.remaining_cycles = cycles;
        self.program_counter = self.program_counter.wrapping_add(jump);
    }

    fn opcode_st(&mut self, register: Register, mode: AddressMode) {
        let (addr, _) = self.instruction_address_from_mode(mode);
        let value = match register {
            Register::Accumulator => self.accumulator,
            Register::X => self.index_x,
            Register::Y => self.index_y,
            Register::Stack => panic!("Invalid register"),
        };
        self.write(addr, value);

        let (jump, cycles) = match mode {
            AddressMode::ZeroPage => (2, 3),
            AddressMode::ZeroPageX | AddressMode::ZeroPageY => (2, 4),
            AddressMode::Absolute => (3, 4),
            AddressMode::AbsoluteX | AddressMode::AbsoluteY => (3, 5),
            AddressMode::IndirectX | AddressMode::IndirectY => (2, 6),
            _ => panic!("Address mode not supported for opcodes 'st'"),
        };

        self.remaining_cycles = cycles;
        self.program_counter = self.program_counter.wrapping_add(jump);
    }

    fn opcode_transfer(&mut self, from: Register, to: Register) {
        let value = match from {
            Register::Accumulator => self.accumulator,
            Register::X => self.index_x,
            Register::Y => self.index_y,
            Register::Stack => self.stackpointer,
        };

        match to {
            Register::Accumulator => self.accumulator = value,
            Register::X => self.index_x = value,
            Register::Y => self.index_y = value,
            Register::Stack => self.stackpointer = value,
        };

        match (from, to) {
            (Register::X, Register::Stack) => {}
            _ => {
                self.status.update_with_value(value);
            }
        };

        self.remaining_cycles = 2;
        self.program_counter = self.program_counter.wrapping_add(1);
    }

    fn opcode_dec_address(&mut self, mode: AddressMode) {
        let (addr, _) = self.instruction_address_from_mode(mode);
        let value = self.read(addr);
        let value = value.wrapping_sub(1);
        self.write(addr, value);

        let (jump, cycles) = match mode {
            AddressMode::ZeroPage => (2, 5),
            AddressMode::ZeroPageX => (2, 6),
            AddressMode::Absolute => (3, 6),
            AddressMode::AbsoluteX => (3, 7),
            _ => panic!("Address mode not supported for dec"),
        };

        self.status.update_with_value(value);
        self.remaining_cycles = cycles;
        self.program_counter = self.program_counter.wrapping_add(jump);
    }

    fn opcode_dec_register(&mut self, register: Register) {
        match register {
            Register::X => self.index_x = self.index_x.wrapping_sub(1),
            Register::Y => self.index_y = self.index_y.wrapping_sub(1),
            _ => panic!("Invalid register"),
        }

        self.status.update_with_value(match register {
            Register::X => self.index_x,
            Register::Y => self.index_y,
            _ => panic!("Invalid register"),
        });
        self.remaining_cycles = 2;
        self.program_counter = self.program_counter.wrapping_add(1);
    }

    fn opcode_inc_address(&mut self, mode: AddressMode) {
        let (addr, _) = self.instruction_address_from_mode(mode);
        let value = self.read(addr);
        let value = value.wrapping_add(1);
        self.write(addr, value);

        let (jump, cycles) = match mode {
            AddressMode::ZeroPage => (2, 5),
            AddressMode::ZeroPageX => (2, 6),
            AddressMode::Absolute => (3, 6),
            AddressMode::AbsoluteX => (3, 7),
            _ => panic!("Address mode not supported for dec"),
        };

        self.status.update_with_value(value);
        self.remaining_cycles = cycles;
        self.program_counter = self.program_counter.wrapping_add(jump);
    }

    fn opcode_inc_register(&mut self, register: Register) {
        match register {
            Register::X => self.index_x = self.index_x.wrapping_add(1),
            Register::Y => self.index_y = self.index_y.wrapping_add(1),
            _ => panic!("Invalid register"),
        }

        self.status.update_with_value(match register {
            Register::X => self.index_x,
            Register::Y => self.index_y,
            _ => panic!("Invalid register"),
        });
        self.remaining_cycles = 2;
        self.program_counter = self.program_counter.wrapping_add(1);
    }

    fn opcode_jmp(&mut self, mode: AddressMode) {
        let (addr, _) = self.instruction_address_from_mode(mode);

        self.remaining_cycles = 3;
        self.program_counter = addr;
    }

    fn opcode_push_accumulator(&mut self) {
        self.stack_push(self.accumulator);

        self.remaining_cycles = 3;
        self.program_counter = self.program_counter.wrapping_add(1);
    }

    fn opcode_pull_accumulator(&mut self) {
        let value = self.stack_pop();
        self.accumulator = value;
        self.status.update_with_value(value);

        self.remaining_cycles = 4;
        self.program_counter = self.program_counter.wrapping_add(1);
    }

    fn opcode_push_status(&mut self) {
        todo!()
    }

    fn opcode_pull_status(&mut self) {
        todo!()
    }

    fn instruction_address_from_mode(&self, mode: AddressMode) -> (u16, BoundaryStatus) {
        match mode {
            AddressMode::Immediate => (
                self.program_counter.wrapping_add(1),
                BoundaryStatus::Uncrossed,
            ),
            AddressMode::ZeroPage => (
                self.read(self.program_counter.wrapping_add(1)).into(),
                BoundaryStatus::Uncrossed,
            ),

            AddressMode::ZeroPageX => (
                u16::from(
                    self.read(self.program_counter.wrapping_add(1))
                        .wrapping_add(self.index_x),
                ),
                BoundaryStatus::Uncrossed,
            ),

            AddressMode::ZeroPageY => (
                u16::from(self.read(self.program_counter.wrapping_add(1)))
                    .wrapping_add(self.index_y.into()),
                BoundaryStatus::Uncrossed,
            ),

            AddressMode::Absolute => {
                let upper = self.read(self.program_counter.wrapping_add(1));
                let lower = self.read(self.program_counter.wrapping_add(2));
                (
                    u16::from_be_bytes([upper, lower]),
                    BoundaryStatus::Uncrossed,
                )
            }
            AddressMode::AbsoluteX => {
                let upper = self.read(self.program_counter.wrapping_add(1));
                let lower = self.read(self.program_counter.wrapping_add(2));

                let (_, boundary_crossed) = lower.overflowing_add(self.index_x);

                let addr = u16::from_be_bytes([upper, lower]);
                let addr = addr + u16::from(self.index_x);

                (
                    addr,
                    if boundary_crossed {
                        BoundaryStatus::Crossed
                    } else {
                        BoundaryStatus::Uncrossed
                    },
                )
            }
            AddressMode::AbsoluteY => {
                let upper = self.read(self.program_counter.wrapping_add(1));
                let lower = self.read(self.program_counter.wrapping_add(2));

                let (_, boundary_crossed) = lower.overflowing_add(self.index_y);

                let addr = u16::from_be_bytes([upper, lower]);
                let addr = addr + u16::from(self.index_y);

                (
                    addr,
                    if boundary_crossed {
                        BoundaryStatus::Crossed
                    } else {
                        BoundaryStatus::Uncrossed
                    },
                )
            }

            _ => todo!(),
        }
    }
}

impl Debug for Processor {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        writeln!(
            f,
            "SP: ${:04X} -> {:02X}",
            u16::from(self.stackpointer) + 0x100,
            self.read(u16::from(self.stackpointer) + 0x100)
        )?;

        writeln!(
            f,
            "PC: ${:04X} -> {:02X}",
            self.program_counter,
            self.read(self.program_counter)
        )?;

        writeln!(f, "A: {:02X}", self.accumulator)?;
        writeln!(f, "X: {:02X}", self.index_x)?;
        writeln!(f, "Y: {:02X}", self.index_y)?;
        writeln!(f, "S: {:?}", self.status)?;

        Ok(())
    }
}

pub enum BoundaryStatus {
    Uncrossed,
    Crossed,
}

#[derive(Clone, Copy)]
pub enum AddressMode {
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Indirect,
    IndirectX,
    IndirectY,
    Relative,
}

enum Register {
    Accumulator,
    X,
    Y,
    Stack,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_opcode_ld() {
        // LDA Intermediate
        let mut processor = Processor::new();
        processor.write(0x0000, 0xA9);
        processor.write(0x0001, 0xEE);
        processor.tick();
        assert_eq!(processor.accumulator, 0xEE);

        // LDA ZeroPage
        let mut processor = Processor::new();
        processor.write(0x0000, 0xA5);
        processor.write(0x0001, 0x02);
        processor.write(0x0002, 0xEE);
        processor.tick();
        assert_eq!(processor.accumulator, 0xEE);

        // LDA ZeroPageX
        let mut processor = Processor::new();
        processor.index_x = 0x03;
        processor.write(0x0000, 0xB5);
        processor.write(0x0001, 0x02);
        processor.write(0x0005, 0xEE);
        processor.tick();
        assert_eq!(processor.accumulator, 0xEE);

        // LDA ZeroPageX Overflow
        let mut processor = Processor::new();
        processor.index_x = 0xFF;
        processor.write(0x0000, 0xB5);
        processor.write(0x0001, 0x03);
        processor.write(0x0002, 0xEE);
        processor.tick();
        assert_eq!(processor.accumulator, 0xEE);

        // LDA Absolute
        let mut processor = Processor::new();
        processor.write(0x0000, 0xAD);
        processor.write(0x0001, 0xBB);
        processor.write(0x0002, 0xCC);
        processor.write(0xBBCC, 0xEE);
        processor.tick();
        assert_eq!(processor.accumulator, 0xEE);

        // LDA AbsoluteX
        let mut processor = Processor::new();
        processor.index_x = 0x02;
        processor.write(0x0000, 0xBD);
        processor.write(0x0001, 0xBB);
        processor.write(0x0002, 0xFF);
        processor.write(0xBC01, 0xEE);
        processor.tick();
        assert_eq!(processor.accumulator, 0xEE);

        // LDA AbsoluteY
        let mut processor = Processor::new();
        processor.index_y = 0x02;
        processor.write(0x0000, 0xB9);
        processor.write(0x0001, 0xBB);
        processor.write(0x0002, 0xFF);
        processor.write(0xBC01, 0xEE);
        processor.tick();
        assert_eq!(processor.accumulator, 0xEE);
    }

    #[test]
    fn test_opcode_st() {
        // STA ZeroPage
        let mut processor = Processor::new();
        processor.accumulator = 0xEE;
        processor.write(0x0000, 0x85);
        processor.write(0x0001, 0x02);
        processor.tick();
        assert_eq!(processor.read(0x0002), 0xEE);
    }

    #[test]
    fn test_opcode_dec() {
        // DEC ZeroPage
        let mut processor = Processor::new();
        processor.write(0x0000, 0xC6);
        processor.write(0x0001, 0x02);
        processor.write(0x0002, 0xFF);
        processor.tick();
        assert_eq!(processor.read(0x0002), 0xFE);
    }

    #[test]
    fn test_opcode_dec_register() {
        // DEX
        let mut processor = Processor::new();
        processor.index_x = 0xEE;
        processor.write(0x0000, 0xCA);
        processor.tick();
        assert_eq!(processor.index_x(), 0xED);
    }

    #[test]
    fn test_push_accumulator() {
        let mut processor = Processor::new();

        processor.accumulator = 0xEE;
        processor.write(0x0000, 0x48);
        processor.write(0x0001, 0x48);
        processor.write(0x0002, 0x48);

        for _ in 0..9 {
            processor.tick();
        }

        assert_eq!(processor.read(0x01FF), 0xEE);
        assert_eq!(processor.read(0x01FE), 0xEE);
        assert_eq!(processor.read(0x01FD), 0xEE);
        assert_eq!(processor.read(0x01FC), 0x00);
    }
}
