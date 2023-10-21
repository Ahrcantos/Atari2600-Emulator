use std::fmt::Debug;

pub struct Status(u8);

impl Status {
    pub fn init() -> Self {
        Self(0b00100000)
    }

    pub fn update_with_value(&mut self, value: u8) {
        if value == 0x00 {
            self.set_zero(true);
        }

        if (value as i8) < 0 {
            self.set_neg(true)
        }
    }

    pub fn set_neg(&mut self, value: bool) {
        if value {
            self.0 |= 0b10000000;
        } else {
            self.0 &= 0b01111111;
        }
    }

    pub fn neg(&self) -> bool {
        self.0 & 0b10000000 != 0x00
    }

    pub fn set_zero(&mut self, value: bool) {
        if value {
            self.0 |= 0b00000010;
        } else {
            self.0 &= 0b11111101;
        }
    }

    pub fn zero(&self) -> bool {
        self.0 & 0b00000010 != 0x00
    }

    pub fn set_carry(&mut self, value: bool) {
        if value {
            self.0 |= 0b00000001;
        } else {
            self.0 &= 0b11111110;
        }
    }

    pub fn carry(&self) -> bool {
        self.0 & 0b00000001 != 0x00
    }

    pub fn set_overflow(&mut self, value: bool) {
        if value {
            self.0 |= 0b01000000;
        } else {
            self.0 &= 0b10111111;
        }
    }

    pub fn overflow(&self) -> bool {
        self.0 & 0b01000000 != 0x00
    }
}

impl Debug for Status {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let mut chars = Vec::new();
        if self.overflow() {
            chars.push("O");
        }

        if self.carry() {
            chars.push("C");
        }

        if self.neg() {
            chars.push("N");
        }

        if self.zero() {
            chars.push("Z");
        }

        if chars.len() != 0 {
            write!(f, "{}", chars.join(" | "))?;
        } else {
            write!(f, "-")?;
        }

        Ok(())
    }
}

impl From<Status> for u8 {
    fn from(value: Status) -> Self {
        value.0
    }
}
