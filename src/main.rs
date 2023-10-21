mod mapper;
mod ppu;
mod processor;

use std::time::Duration;

use processor::Processor;

use sdl2::{pixels::Color, rect::Rect, render::Canvas, ttf::Font, video::Window};

fn main() {
    let mut processor = Processor::new();
    processor.write(0x0000, 0xE8);
    processor.write(0x0001, 0x8A);
    processor.write(0x0002, 0x48);
    processor.write(0x0003, 0x48);
    processor.write(0x0004, 0x48);
    for i in 0..(0xFF - 0x04) {
        processor.write(i + 0x0004, 0xEA);
    }

    let sdl_context = sdl2::init().unwrap();
    let video_context = sdl_context.video().unwrap();

    let window = video_context
        .window("Nesemu", 800, 600)
        .position_centered()
        .build()
        .unwrap();

    let mut canvas = window.into_canvas().build().unwrap();
    let mut event_pump = sdl_context.event_pump().unwrap();
    let ttf_context = sdl2::ttf::init().unwrap();
    let font = ttf_context
        .load_font("assets/ttf/PressStart.ttf", 10)
        .unwrap();

    canvas.clear();
    canvas.present();

    'running: loop {
        canvas.clear();

        render_processor_registers(&mut canvas, &font, &processor);
        render_processor_memory(&mut canvas, &font, &processor);

        std::thread::sleep(Duration::from_micros(1));

        for event in event_pump.poll_iter() {
            match event {
                sdl2::event::Event::Quit { .. } => break 'running,
                _ => {}
            }
        }
        processor.tick();
        canvas.present();
    }
}

fn render_processor_memory(canvas: &mut Canvas<Window>, font: &Font, processor: &Processor) {
    let texture_creator = canvas.texture_creator();

    for addr in 0x00..0x200 {
        let x = addr % 16;
        let y = addr / 16;

        let color = {
            if addr == processor.program_counter() {
                Color::RGB(0x24, 0xA2, 0xBF)
            } else {
                Color::WHITE
            }
        };
        let text = format!("{:02X}", processor.read(addr));
        let surface = font.render(&text).solid(color).unwrap();
        let texture = surface.as_texture(&texture_creator).unwrap();
        let rect = Rect::new(
            i32::from(x) * surface.width() as i32,
            (i32::from(y) * surface.height() as i32) + 90,
            surface.width(),
            surface.height(),
        );

        canvas.copy(&texture, None, rect).unwrap();
    }
}

fn render_processor_registers(canvas: &mut Canvas<Window>, font: &Font, processor: &Processor) {
    let texture_creator = canvas.texture_creator();

    // Stackpointer
    {
        let stackpointer_addr = format!(
            "${:04X} ({:02X})",
            processor.stackpointer(),
            processor.stack_top()
        );

        let surface_addr = font.render(&stackpointer_addr).solid(Color::WHITE).unwrap();
        let texture_addr = surface_addr.as_texture(&texture_creator).unwrap();
        let rect_addr = Rect::new(45, 5, surface_addr.width(), surface_addr.height());

        let color = Color::RGB(0xBF, 0x24, 0x7C);
        let surface_label = font.render("SP").solid(color).unwrap();
        let texture_label = surface_label.as_texture(&texture_creator).unwrap();
        let rect_label = Rect::new(5, 5, surface_label.width(), surface_label.height());

        canvas.copy(&texture_addr, None, rect_addr).unwrap();
        canvas.copy(&texture_label, None, rect_label).unwrap();
    }

    // Program counter
    {
        let color = Color::RGB(0x24, 0xA2, 0xBF);
        let surface_label = font.render("PC").solid(color).unwrap();
        let texture_label = surface_label.as_texture(&texture_creator).unwrap();
        let rect_label = Rect::new(5, 20, surface_label.width(), surface_label.height());

        canvas.copy(&texture_label, None, rect_label).unwrap();

        let program_counter_addr = format!(
            "${:04X} ({:02X})",
            processor.program_counter(),
            processor.read(processor.program_counter())
        );

        let surface_addr = font
            .render(&program_counter_addr)
            .solid(Color::WHITE)
            .unwrap();
        let texture_addr = surface_addr.as_texture(&texture_creator).unwrap();

        let rect_addr = Rect::new(45, 20, surface_addr.width(), surface_addr.height());

        canvas.copy(&texture_addr, None, rect_addr).unwrap();
    }

    // Accumulator
    {
        let color = Color::RGB(0xBF, 0x24, 0x5A);
        let surface_label = font.render("ACC").solid(color).unwrap();
        let texture_label = surface_label.as_texture(&texture_creator).unwrap();
        let rect_label = Rect::new(5, 35, surface_label.width(), surface_label.height());
        canvas.copy(&texture_label, None, rect_label).unwrap();

        let surface_value = font
            .render(&format!("{:02X}", processor.accumulator()))
            .solid(Color::WHITE)
            .unwrap();
        let texture_value = surface_value.as_texture(&texture_creator).unwrap();
        let rect_value = Rect::new(45, 35, surface_value.width(), surface_value.height());
        canvas.copy(&texture_value, None, rect_value).unwrap();
    }

    // IndexX
    {
        let color = Color::RGB(0xC6, 0xC6, 0x19);
        let surface_label = font.render("X").solid(color).unwrap();
        let texture_label = surface_label.as_texture(&texture_creator).unwrap();
        let rect_label = Rect::new(5, 50, surface_label.width(), surface_label.height());
        canvas.copy(&texture_label, None, rect_label).unwrap();

        let surface_value = font
            .render(&format!("{:02X}", processor.index_x()))
            .solid(Color::WHITE)
            .unwrap();
        let texture_value = surface_value.as_texture(&texture_creator).unwrap();
        let rect_value = Rect::new(45, 50, surface_value.width(), surface_value.height());
        canvas.copy(&texture_value, None, rect_value).unwrap();
    }
    // IndexY
    {
        let color = Color::RGB(0xC6, 0x70, 0x19);
        let surface_label = font.render("Y").solid(color).unwrap();
        let texture_label = surface_label.as_texture(&texture_creator).unwrap();
        let rect_label = Rect::new(5, 65, surface_label.width(), surface_label.height());
        canvas.copy(&texture_label, None, rect_label).unwrap();

        let surface_value = font
            .render(&format!("{:02X}", processor.index_y()))
            .solid(Color::WHITE)
            .unwrap();
        let texture_value = surface_value.as_texture(&texture_creator).unwrap();
        let rect_value = Rect::new(45, 65, surface_value.width(), surface_value.height());
        canvas.copy(&texture_value, None, rect_value).unwrap();
    }
}
