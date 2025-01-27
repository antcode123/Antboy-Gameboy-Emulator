# `Gameboy Emulator - Key Algorithms`

This document defines the key algorithms, along with explanations, of the different hardware components that need to be emulated to create a Gameboy Emulator. The style of pseudocode is a custom style which incorporates a mix of structured English and programming key words.

---

# Gameboy Algorithms

### `Gameboy::emulate()`
The `emulate` procedure is the core of the emulation. Firstly, the ticks threshold for one frame is dynamically generated based on the emulator's FPS, ensuring FPS independence. During one iteration of the emulation cycle, the CPU fetches, decodes and executes an instruction. After the number of ticks taken for that instruction to execute has been returned, these ticks are added onto the total elapsed ticks for a single frame and will be used to clock the other components such as the PPU, APU and timer. At the end of the iteration, interrupts are handled. This cycle repeats until the total elapsed ticks reaches the threshold for the current frame.

```pseudo
Gameboy::emulate():
    CALCULATE the ticks threshold for the current frame by doing the clock speed / the FPS
    WHILE the CPU ticks is less than the ticks threshold:
        RUN the CPU and and obtain the number of ticks that the last CPU instruction took to execute
        USE the last instruction ticks to clock the other components (PPU and Timer)
        HANDLE interrupts
    RESET the clocks
```

### `Gameboy::update_fps()`
The `update_fps` procedure calculates the delta time between frame and updates the fps using the delta time. If the frame runs too fast, the program will sleep until the expected delta time is reached.

```pseudo
Gameboy::update_fps():
    GET the current time in seconds
    CALCULATE the delta time by subtracting the previous time from the current time
    IF the delta time is less than the expected delta time:
        CALCULATE the wait time by subtracting the delta time by the expected delta time
        SLEEP the program by the wait time
        GET the current time again
        SET the previous time to the current time
    ELSE: SET the previous time to the current time
    CALCULATE the fps by doing 1 / the delta time
```


### `Gameboy::update_fps()`
The `update_fps` procedure calculates the delta time between frame and updates the fps using the delta time. If the frame runs too fast, the program will sleep until the expected delta time is reached.

```pseudo
Gameboy::update_fps():
    GET the current time in seconds
    CALCULATE the delta time by subtracting the previous time from the curren time
    IF the delta time is less than the expected delta time:
        CALCULATE the wait time by subtracting the delta time by the expected delta time
        SLEEP the program by the wait time
        GET the current time again
        SET the previous time to the current time
    ELSE: SET the previous time to the current time
    CALCULATE the fps by doing 1 / the delta time
```

### `Gameboy::load_settings()`
The `load_settings` procedure loads the user's settings from the settings.json file.

```pseudo
Gameboy::load_settings():
    TRY:
        OPEN then settings.json file
        READ the contents of the json file and map the settings to gameboy member variables
    EXCEPT:
        LOAD the default settings
```

## `CPU Algorithms`

### `CPU::run()`
The `run` procedure performs a single iteration of the Fetch-Decode-Execute Cycle. When the CPU is halted, it doesn't nothing other than clock 4 ticks. Otherwise, it fetches the next instruction from memory, decodes, and executes it, returning the number of ticks the instruction took.

```pseudo
CPU::run():
    IF the cpu is halted: RETURN 4 ticks
    FETCH instruction from memory and decode/execute it
    RETURN the number of ticks the instruction took
```

### `CPU::fetch()`
The `fetch` function retrieves the next opcode from memory by reading the value at the program counter. The program counter is then incremented to point to the next instruction or immediate value.

```pseudo
CPU::fetch():
    READ the next opcode from memory at the program counter
    INCREMENT the program counter by 1
    RETURN the fetched opcode
```

### `CPU::execute(U8 opcode)`
The `execute` function decodes the fetched opcode and performs the corresponding operation (e.g., load, store, arithmetic, bit manipulation, control flow). The number of ticks the instruction took is then returned.

```pseudo
CPU::execute(U8 opcode):
    DECODE opcode
    PERFORM the corresponding operation
    RETURN the ticks taken for that instruction
```

### `CPU::handle_interrupts()`
The `handle_interrupts` procedure manages the CPU's interrupt handling. It checks which interrupts are active by masking the interrupt flag and interrupt enable registers. If any interrupts are enabled, the CPU is is unhalted. If the interrupt master is enabled, the CPU services the enabled interrupts in priority order.

```pseudo
CPU::handle_interrupts():
    DETERMINE active interrupts by masking the interrupt flag register and the interrupt enabled register
    IF there are no active interrupts: RETURN
    SET is_halted to false
    IF interrupt master enable flag is false: RETURN
    CHECK through each interrupt in priority order and handles its service routine if the corresponding interrupt flag is enabled in the interrupt flag register
```

---

## `Cartridge Algorithms`

### `Cartridge::configure()`
The `configure` procedure reads the cartridge's metadata to determine the MBC, ROM size and RAM size. It calculates the total number of ROM banks and loads any saved RAM data.

```pseudo
Cartridge::configure():
    DETERMINE the MBC from the cartridge header
    DETERMINE the ROM size from the cartridge header
    DETERMINE the RAM size from the cartridge header
    CALCULATE the total ROM banks from the ROM size
    LOAD the saved RAM
```

### `Cartridge::read_rom(U16 address)`
The `read_rom` function reads data from the ROM. The address is mapped to the appropriate ROM bank depending on the MBC.

```pseudo
Cartridge::read_rom(U16 address):
    SET a temporary ROM bank and set it to 0
    IF the address is less 0x4000:
        MODIFY the temporary ROM bank based on the MBC and address
        RETURN the data stored at the temporary ROM bank
    WRAP the temporary ROM bank around the total ROM banks
    MODIFY the temporary ROM bank based on the MBC
    RETURN the data stored at the temporary ROM bank
```

### `Cartridge::read_ram(U16 address)`
The `read_ram` function reads data from the cartridge's RAM if enabled. Different MBCs handle ram reading differently

```pseudo
Cartridge::read_ram(U16 address):
    RETURN if RAM isn't enabled
    IF the MBC is MBC1:
        IF the MBC1 is in ROM bank mode: SET a temporary RAM bank to 0
        ELSE: SET the temporary RAM bank to the MBC1 RAM bank
        IF the RAM size is 0: RETURN 0xFF   `
        IF the RAM size is less than or equal to 8192: RETURN the data stored at the mapped address wrapped around the RAM size
        ELSE: RETURN the data stored at the temporary RAM bank
    IF the MBC is MBC2:
        SET the upper nibble of the data stored at the mapped address to 1
        RETURN the modified data stored at the mapped address
    RETURN 0xFF for any other MBC as they are not implemented
```

---

## `MBC Algorithms`

### `MBC1::write(U16 address, U8 u8)`
The `write` procedure for MBC1 controls what happens when the cartridge ROM is written to. It can either enable/disable RAM, modify ROM/RAM banks or modify the bank mode.

```pseudo
MBC1::write(U16 address, U8 u8):
    IF the address is less than 0x2000:
        IF the lower nibble of the u8 value is 0xA: ENABLE RAM
        ELSE: DISABLE RAM
    ELIF the address is less than 0x4000:
        MASK off the upper 3 bits from the u8 value
        IF the u8 value is 0: SET the u8 value to 1
        SET the lower 5 bits of the ROM bank to the u8 value
    ELIF the address is less than 0x6000:
        MASK off the upper 6 bits from the u8 value
        IF the MBC1 is in RAM bank mode: SET the ram bank to the u8 value
        ELSE: SET bits 6 and 5 of the ROM bank to the u8 value
    ELIF the address is less than 0x8000: SET the MBC1 bank mode to bit 0 of the u8 value
```

### `MBC1::modify_rom_bank(U16 address, REFERENCE<U8> temporary_rom_bank)`
The `modify_rom_bank` procedure modifies a temporary ROM bank based on the MBC1 mode and total ROM size.

```pseudo
MBC1::modify_rom_bank(U16 address, REFERENCE<U8> temporary_rom_bank):
    IF the MBC1 is in ROM bank mode or the total ROM banks is less than 32: RETURN
    IF the total ROM banks is either 64 or 128: SET bit 5 of the temporary ROM bank to bit 0 of the RAM bank
    IF the total ROM banks is 128: SET bit 6 of the temporary ROM bank to bit 1 of the RAM bank
```

### `MBC2::write(U16 address, U8 u8)`
The `write` procedure for MBC2 handles RAM enabling and ROM bank switching.

```pseudo
MBC2::write(U16 address, U8 u8):
    IF the address is greater than 0x4000: RETURN
    IF bit 8 of the address is 1: SET the ROM bank to the lower nibble of the u8 value
    ELIF the address is less than 0x2000:
        IF the lower nibble of the u8 value is 0xA: ENABLE RAM
        ELSE: DISABLE RAM
    IF the ROM bank is 0: INCREMENT the rom bank by 1
```

---

## `Joypad Algorithms`

### `Joypad::read()`
The `read` function returns the current state of the joypad buttons. Depending on the button type selected within joypad register, it returns either the lower or upper nibble of the button states. The upper nibble stores the directional keys whilst the lower nibble stores the action buttons.

```pseudo
Joypad::read():
    IF bit 5 of the joypad register is 1: SET the lower nibble of the joypad register to the lower nibble of joypad button states
    ELSE: SET the lower nibble of the joypad register to the upper nibble of joypad button states
    RETURN the joypad register
```

### `Joypad::press_button(INT button_bit)`
The `press_button` procedure sets a specific button bit to false (the state for a button pressed) and triggers a joypad interrupt.

```pseudo
Joypad::press_button(INT button_bit):
    SET the button bit of the joypad button states to false
    TRIGGER the joypad interrupt
```

### `Joypad::check_key_pressed(REFERENCE<EVENT> event, JSON key_binds)`
The `check_key_pressed` procedure checks if a key press event matches a key binding and presses the corresponding button.

```pseudo
Joypad::check_key_pressed(REFERENCE<EVENT> event, JSON key_binds):
    IF the event key code maps to the bind for DOWN: press_button(7)
    IF the event key code maps to the bind for UP: press_button(6)
    IF the event key code maps to the bind for LEFT: press_button(5)
    IF the event key code maps to the bind for RIGHT: press_button(4)
    IF the event key code maps to the bind for START: press_button(3)
    IF the event key code maps to the bind for SELECT: press_button(2)
    IF the event key code maps to the bind for B: press_button(1)
    IF the event key code maps to the bind for A: press_button(0)
```

---

## `LCD Algorithms`

### `LCD::transfer_pixel(INT x, INT y, U8 pixel)`
The `transfer_pixel` procedure transfers a single pixel's value to the frame buffer. The 2D coordinates are mapped to 1D coordinates as the framer buffer is 1 dimentional for optimisation.

```pseudo
LCD::transfer_pixel(INT x, INT y, U8 pixel):
    CALCULATE the pixel location within the frame buffer by multiplying y by the width then adding x
    WRITE the pixel to the last frame buffer at the pixel location
```

### `LCD::update_frame_buffers()`
The `update_frame_buffers` procedure updates the LCD's frame buffers by removing the oldest frame and pushing a new frame onto the back - simulating a history or queue of frame buffers (used for the retro mode effect)

```pseudo
LCD::update_frame_buffers():
    POP the oldest frame from the front of the frame buffers
    PUSH a new frame onto the back of the frame buffers
```

### `LCD::display(REFERENCE<WINDOW> window, ARRAY<COLOR, 4> palette)`
Renders the current frame buffer to the display, applying any necessary transformations such as scaling or retro mode blending. When in retro mode, a mean pixel is calculated from blending corresponding pixels within every frame buffer (apart from the second to last). Using a vertex array to render the frame buffer is an optimisation design used to signficantly speed up rendering of many pixels as it reduces the number of draw calls to the GPU.

```pseudo
LCD::display(REFERENCE<WINDOW> window, ARRAY<COLOR, 4> palette):
    SET pixel index and set it to 0
    FOR i from 0 to height:
        FOR i from 0 to width:
            CALCULATE the pixel location within the frame buffer by multiplying y by the width then adding x
            IF retro mode is enabled:
                BLEND the pixel at the pixel location
                SET the pixel color to the blended pixel
            ELSE:
                GET the pixel from the second to last frame buffer at the pixel location
                SET the pixel color to the color within the palette at index pixel
            Calculate the 2D pixel window offset
            SCALE the pixel within the frame buffer vertex array
            INCREMENT the pixel index by 4
    DRAW the frame buffer vertex array to the window
```

### `LCD::blend_pixel(INT location, ARRAY<COLOUR, 4>> palette)`
The 'blend_pixel` function takes an average of corresponding pixels in the frame buffers to create a retro mode effect.

```pseudo
LCD::blend_pixel(INT location, ARRAY<COLOUR, 4>> palette):
    SET the blended RGB channels to 0

    FOR i from 0 to total frame buffers - 1:
        GET the pixel from the second to last frame buffer at the index i
        SET the pixel color to the color within the palette at index pixel
        INCREMENT each RGB channel by the the corresponding RGB channel within the pixel color

    DIVIDE each channel by the total frame buffers - 1
    MERGE each channel into a single blended pixel
    RETURN the blended pixel
```

---

## `MMU Algorithms`

### `MMU::read_u8(U16 address)`
This function reads an 8-bit value from the memory map, handling reads from the cartridge and different regions of ROM, RAM and hardware registers.

```pseudo
MMU::read_u8(U16 address):
    IF the bootstrap is enabled and the address is less than 0x100: RETURN the value stored within the boostrap at the address
    IF the address is less than 0x8000: RETURN the value returned from the cartridge read ROM function at the address
    IF the address is less than 0xC000: RETURN the value within video RAM at the mapped address
    IF the address is less than 0xA000: RETURN the value returned from the cartridge read RAM function at the address
    IF the address is less than 0xE000: RETURN the value within work RAM at the mapped address
    IF the address is greater than or equal to 0xFE00 and less than or equal to 0xFEA0: return the value within OAM at the mapped address
    IF the address is 0xFF00: RETURN the value returned from the joypad read function
    IF the address is greater than or equal to FF04 and less than 0xFF08: RETURN the value returned from the timer read function at the address
    IF the address is 0xFF0F: RETURN the interrupt flag register
    IF the address is greater than or equal to FF10 and less than 0xFF30: RETURN 0 to prevent APU related bugs
    IF the address is greater than or equal to FF40 and less than 0XFF50: RETURN the value returned from the PPU read function at the address
    IF the address is greater than or equal to 0xFF80 and less than 0xFFFF: RETURN the value within high RAM at the mapped address
    IF the address is 0xFFFF: RETURN the interrupt enabled register
    RETURN 0xFF for any other unmapped address
```

### `MMU::read_u8(U16 address)`
This function reads an 16-bit value from the memory map (this is just consecutive reads). The Gameboy using little-endian so the most significant byte is stored in the high address and the least significant in the low address.

```pseudo
MMU::read_u16(U16 address):
    SET the upper byte of the u16 value to the value returned from the read u8 function at the address + 1
    SET the lower byte of the u16 value to the value returned from the read u8 function at the address
    RETURN the u16 value
```

### `MMU::write_u8(U16 address, U8 u8)`
The `write_u8` procedure writes a byte to the gameboy memory map.
```pseudo
MMU::write_u8(U16 address, U8 u8):
    IF the address is less than 0x8000: WRITE the u8 value to the cartridge MBC at the address
    IF the address is less than 0xC000: WRITE the u8 value to video RAM at the mapped address
    IF the address is less than 0xA000: WRITE the u8 value to cartridge RAM
    IF the address is less than 0xE000: WRITE the u8 value to work RAM at the mapped address
    IF the address is greater than or equal to 0xFE00 and less than or equal to 0xFEA0: WRITE the u8 value to OAM at the mapped address
    IF the address is 0xFF00: WRITE the u8 value to the upper nibble of the joypad register
    IF the address is greater than or equal to FF04 and less than 0xFF08: WRITE the u8 value to timer at the address
    IF the address is 0xFF0F: WRITE the u8 value to the interrupt flag register
    IF the address is greater than or equal to FF40 and less than 0XFF50:
        IF the address is 0xFF46: PERFORM DMA transfer with the encoded source address stored within the u8 value
        ELSE: WRITE the u8 value to PPU at the address
    IF the address is 0xFF50: DISABLE the bootstrap
    IF the address is greater than or equal to 0xFF80 and less than 0xFFFF: WRITE to high RAM at the mapped address
    IF the address is 0xFFFF: WRITE the u8 value to the interrupt enabled register
```

### `MMU::perform_dma_transfer(U16 encoded_source_address)`
The `perform_dma_transfer` procedure transfers data from the source address to OAM. Before the transfer is performed, the source address must be decoded by bit shifting by to the left by 8.

```pseudo
MMU::perform_dma_transfer(U8 encoded_source_address)
    BIT shift the encoded source address left by 8 to get the actual source address
    FOR i from 0 to 160: TRANSFER data from the source address at source address + i into OAM at its corresponding location
```

---

## `PPU Algorithms`

### `PPU::read(U16 address)`
The `read` retrieve different PPU registers like LCD control, status, and other graphics control data.

```pseudo
PPU::read(U16 address):
    IF the address is 0xFF40: RETURN the LCD control register
    IF the address is 0xFF41: RETURN the LCD status register
    IF the address is 0xFF42: RETURN the scroll y register
    IF the address is 0xFF43: RETURN the scroll x register
    IF the address is 0xFF44: RETURN the scanline y register
    IF the address is 0xFF45: RETURN the scanline y comparison register
    IF the address is 0xFF47: RETURN the background palette register
    IF the address is 0xFF48: RETURN the object palette 0 register
    IF the address is 0xFF49: RETURN the object palette 1 register
    IF the address is 0xFF4A: RETURN the window y register
    IF the address is 0xFF4B: RETURN the window x register
    ELSE: RETURN 0xFF
```

### `PPU::run(INT last_instruction_ticks)`
Runs the PPU as a state machine, progressing through 4 different modes: h-blank, v-blank, OAM search and pixel transfer. The PPU is halted when the LCD isn't enabled.

```pseudo
PPU::run(INT last_instruction_ticks):
    INCREMENT the ticks by the last instruction ticks
    IF the LCD is not enabled: RETURN
    IF the mode is h-blank: RUN h-blank
    IF the mode is v-blank: RUN v-blank
    IF the mode is OAM search: RUN OAM search
    IF the mode is pixel transfer: RUN pixel transfer
```

### `PPU::run_pixel_transfer()`
The `run_pixel_transfer` is one of the 4 run mode procedures executed by the PPU. It waits until the pixel tranfer interval has been exeeded, before rendering a scanline and switching to the next mode, h-blank. The LCD status interrupt will trigger if the h-blank status interrupt is enabled within the LCD status register.

```pseudo
PPU::run_pixel_transfer():
    IF the ticks is less than the pixel transfer interval: RETURN
    RENDER the current scanline
    RESET the ticks
    SET the mode to h-blank
    IF the h-blank status interrupt is enabled: TRIGGER LCD status interrupt
```

### `PPU::get_tile_location(INT U8 tile_index)`
The `get_tile_location` function gets the address of the tile the ppu is currently processing. If the addressing mode is unsigned, tile location is addressed from the base pointer 0x8000 using unsigned addressing. If the addressing mode is unsigned, the tile location is addressed from the base pointer 0x9000 using signed addressing. The unsigned operation of adding 128 to the tile index then using the base pointer 0x8800 is another way of performing the signed addressing.

```pseudo
PPU::get_tile_location(U8 tile_index):
    IF unsigned tile addressing is enabled: RETURN 0x8000 + tile_index
    INCREMENT the tile index by 128
    RETURN 0x8800 + tile_index * 16
```

### `PPU::render_background()`
The most technical algorithm I have implemented. The `render_background_algorithm` Renders the background tiles onto the screen by calculating tile positions, fetching tile data, extracting pixels from the tile data and transferring pixels to the LCD.

*Important notes*:
- The background y represents the x position of the current scanline pixel within the background map.
- The tile row represents the row in which th ecurrent pixel is contained, within the background map.
- The bit shifts by 3 and MOD 8 operations are carried out because tiles are 8x8 in size.
- Tiles are only referenced (using the tile index) within the background map
- Tiles can be address using either signed or unsigned addressing.
- Tiles exist as 32 bytes contiguously in tileset memory (every 2 bytes is a single line/row within the tile). Pixels aren't stored within a single byte as you may expect. The high bit of each pixel is stored in the first byte, and the low bit of each each pixel is stored in the subsequent byte.
- The pixels stored within the tiles aren't even true pixel values. They are color IDs used by the color palette to determine the pixel color.
- The color IDs within the background must be cached to determine priority between the background and objects.

```pseudo
PPU::render_background():
    IF the tile map 1 is enabled: SET the tile map offset to 0x9C00
    ELSE: SET the tile map offset to 0x9800
    CALCULATE the background y by summing the scanline y and the scroll y
    CALCULATE the tile row by shifting the background y by 3 bits to the right
    FOR i from 0 to 160:
        SET the scanline x to i
        CALCULATE the background x by summing the scanline x and the scroll x
        CALCULATE the tile column by bit shifting the background x by 3 bits to the right
        CALCULATE the tile index address by doing: tile map offset + tile row * 32 + tile column
        GET the tile index by reading from the address space at the tile index address
        GET the tile address using the tile index
        CALCULATE the tile line address by doing: tile address + 2 * (background y % 8)
        GET the tile line by reading from the address space at the tile line address
        SEPARATE the tile line into high and low bytes
        CALCULATE the pixel bit from in which the current pixel color ID lies by doing: 7 - (background y % 8)
        CALCULATE the pixel bit mask by bit shifting 1 to the right by pixel bit
        MASK both the high and low tile line bytes with the pixel bit mask
        GET the high pixel bit by shifting the high tile line byte by the pixel bit
        GET the low pixel bit by shifting the low tile line byte by the pixel bit
        MERGE the high and low pixel bits into a 2-bit value to form the pixel color ID
        GET the pixel's color using the color ID and background palette
        TRANSFER the pixel to the LCD
        UPDATE the current scanline buffer for object background priority
```

---

## `Timer Algorithms`

### `Timer::read(U16 address)`
The `read` function retrieves the current values of the timer registers, such as the divider or timer counter.

```pseudo
Timer::read(U16 address):
    IF the address is 0xFF04: RETURN the divider
    IF the address is 0xFF05: RETURN the timer counter
    IF the address is 0xFF06: RETURN the timer modulo
    IF the address is 0xFF07: RETURN the timer control
```

### `Timer::run(INT last_instruction_ticks)`
The `run` function increments the by the ticks of the last instruction and runs through both registers.

```pseudo
Timer::run(INT last_instruction_ticks):
    INCREMENT both the divider and counter ticks by the last instruction ticks
    UPDATE both the divider and counter registers
```

### `Timer::update_divider()`
The `update_divider` procedure updateds the divider register by resetting the divider ticks and incrementing the register if the required period has elapsed.

```pseudo
Timer::update_divider():
    IF the divider ticks is less than the divider period: RETURN
    RESET the divider ticks
    INCREMENT the divider
```

### `Timer::update_counter()`
The `update_counter` procedure updates the counter register. The algorithm iteratively increments the counter for as long as the counter ticks is greater than the counter period. This allows timer increments for cases where the last CPU instruction took many ticks and the amount could be large enough that two or more counter incrememets could occurred. If the counter overflows, it resets to the modulo value and triggers an interrupt.

```pseudo
Timer::update_counter():
    IF the counter is not enabled: RETURN
    WHILE the counter ticks is greater than or equal to the counter period:
        RESET the counter ticks
        INCREMENT the counter
        IF the counter is 0:
            SET the counter to the modulo
            TRIGGER the timer interrupt
```

### `StateManager::run()`
The `run` procedure is responsible for the rendering and processing of the current state. The current state is obtained from the top of the state stack and the game loop for that current state is executed. Fades into and out of the state will occur when the user switches a state. Transitioning of states will also be checked within this procedure.
```pseudo
StateManager::run():
    WHILE the current state stack is not empty and the window is open:
        GET the current state from the top of the stack
        update the fps
        IF the current state is running and its screen fade opacity is greater than 0: FADE into the current state
        ELIF the current state is not running and its screen fade opacity is less than 255: FADE out of the current state
        ELIF the current state is running: RUN the current state
        ELSE: SWITCH the current state
```

### `StateManager::fade_in_or_out_of_state(BOOL is_fading_in)`
The `fade_in_or_out_of_state` procedure is responsible for fading into or out of a state. All input from the user is ignored when a state is fading in or out.
```pseudo
StateManager::fade_in_or_out_of_state(BOOL is_fading_in):
    IF is_fading_in: FADE into the current state
    ELSE: FADE out of the current state
    POLL and ignore window events
    PERFORM logic for the current state
    RENDER the current state
```

### `StateManager::run_current_state()`
The `run_current_state` procedure is responsible for rendering and processing .
```pseudo
StateManager::run_current_state():
    HANDLE events for the current state
    PERFORM logic for the current state
    RENDER the current state
```

### `StateManager::switch_current_state()`
The `switch_current_state` procedure is responsible for rendering and processing .
```pseudo
StateManager::switch_current_state():
    IF the current state has entered a new state: PUSH the new state onto the state stack
    ELSE POP the current state from the state stack and reset the new current state
```

### `State::update_cursor(INT direction)`
The `update_cursor` procedure is responsible for updating the cursor pointing to the current UI element. This procedure will carry out scrolling when the there are more than 9 UI elements within a state and the cursor is at the center of the screen (5th from the 9 buttons).
```pseudo
StateManager::update_cursor(INT direction):
        RESET the UI element at the cursor
        INCREMENT or decrement the cursor based on the direction
        CONSTRAIN the cursor between 0 and the number of UI elements - 1
        IF the direction is down and the cursor is the 5th from the 9 active UI elemtents: SCROLL the UI elements up
        IF the direction is up and the cursor is the 5th from the 9 active UI elemtents: SCROLL the UI elements down
        UPDATE the selected UI element of the new cursor position
```

### `State::fade_in()`
The `fade_in` procedure is responsible for fading into the state
```pseudo
StateManager::fade_in():
    SET the animation rate to a scalar multiple of the delta time
    DECREMENT the sceen_fade_opacity by the animation rate
    CONSTRAIN the animation rate to be no less than 0
```

### `UIElement::animate()`
The `animate` procedure is responsible for animating a UI element's scale factor
```pseudo
UIElement::animate():
    SET the animation rate to a scalar multiple of the delta time
    IF the UI element is selected:
        INCREMENT the animation scale factor by the animation rate
        CONSTRAIN the animation scale factor to be no more than a given scale
    ELSE:
        DECREMENT the animation scale factor by the animation rate
        CONSTRAIN the animation scale factor to be no less than 1

```

### `ArrowSelector::handle_right_event()`
The `handle_right_event` procedure is responsible for toggling right through the arrow selector UI element.
```pseudo
ArrowSelector::handle_right_event():
    INCREMENT the cursor and wrap the cursor back to 0 if it exceeds the number of toggle options - 1
    UPDATE the current selection string
```

### `BindRemapper::animate(double delta_time)`
The `animate` procedure is responsible for creating the pulsing 'press any key' animation when the user is selecting a new key to remap.
```pseudo
BindRemapper::animate(double delta_time):
    ANIMATE the size of the bind remapper as you would a typical UI element

    IF the bind remapper is binding:
        INCREMENT the binding animation counter by a scalar multiple of the delta time
        SET wrap the binding animation counter back to 0 when it exceeds 2π
        MAP the oscillating counter to a binding animation opacity range from 0 - 255 by doing 255 * 0.5 * (cos(the binding animation counter) + 1)

    ELSE set the binding animation counter to 2π
```

### `ColorPicker::generate_color_grid()`
The `generate_color_grid` procedure is responsible for generating the grid of color gradients for the color picker. This works by lerping between adjacent colors of the rainbow horizontally for smooth hue transitions and then lerping the result between white and black for smooth shade transitions.
```pseudo
ColorPicker::generate_color_grid():
    INITIALISE the color array with the colors of the rainbow
    SET the grid width to be the horizontal resolution * (the number of colorsh - 1) + 1
    SET the grid heigh to the vertical resolution * 2 + 1

    FOR i from 0 to grid height:
        FOR j from 0 to grid width:
            SET the initial color pointer to j / horizontal resolution
            SET the target color pointer to the initial color pointer + 1
            SET the initial color to the color of at the initial color pointer
            SET the target color to the color at the target color pointer
            SET the lerp factor to (j % the horizontal resolution) / the horizontal resolution
            LERP the target and initial color by the lerp factor
            SET the lerp factor to (i % the vertical resolution) / the vertical resolution
            IF i is less than the vertical resolution: LERP the lerped color with white by the lerp factor
            ELIF i is less than the vertical resolution * 2: lerp the lerped color with oblack by the lerp factor
            ELSE: SET the lerped color to black
```
