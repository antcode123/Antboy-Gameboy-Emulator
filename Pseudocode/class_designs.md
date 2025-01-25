# `Gameboy Emulator - Class Designs`

This document defines all templates used for classes within my emulator, along with brief explanations of their functionality.

---

## `CPU`

The CPU most important component to accurately emulate, responsible for executing instructions, managing registers, and handling interrupts. The drives the emulation - all other components are clocked by the ticks taken by the CPU's last instruction.

```pseudo
CLASS CPU:
    ENUM Flag : ZERO, SUB, HALF_CARRY, CARRY
    ENUM FlagCondition : IS_ZERO, IS_NOT_ZERO, IS_CARRY, IS_NOT_CARRY
    ENUM Combined : AF, BC, DE, HL
    U8 A
    U8 B
    U8 C
    U8 D
    U8 E
    U8 F
    U8 H
    U8 L
    INT clock_speed
    INT ticks
    INT last_instruction_ticks
    BOOL is_interrupt_master_enabled
    BOOL is_halted
    U16 program_counter
    U16 stack_pointer
    REFERENCE<MMU> mmu
    REGISTERS registers

    CONSTRUCTOR CPU(REFERENCE MMU _mmu)
    PROCEDURE reset()
    PROCEDURE run()
    FUNCTION<U16> read_u16(Combined combined_register)
    PROCEDURE write_u16(Combined combined_register, U16 u16)
    PROCEDURE set_flag(Flag flag, BOOL state)
    FUNCTION<BOOL> get_flag(Flag flag)
    PROCEDURE push_onto_stack(U16 u16)
    FUNCTION<U8> pop_off_stack()
    PROCEDURE set_interrupt(U8 interrupt_bit, BOOL state)
    PROCEDURE handle_interrupts()
    PROCEDURE call_interrupt_service_routine(U8 interrupt_bit)
    FUNCTION<U8> fetch()
    FUNCTION<INT> execute(U8 opcode)
```

---

## `Cartridge`

The Cartridge class represents the Gameboy's game cartridge. It manages the game's ROM and RAM data, handles battery-backed save states, and determines the MBC.

```pseudo
CLASS Cartridge:
    ARRAY<U8> rom
    ARRAY<U8> ram
    INT rom_size
    INT ram_size
    BOOL does_contain_battery
    BOOL is_saved
    MBC mbc
    STRING file_path

    PROCEDURE reset()
    PROCEDURE configure()
    FUNCTION<U8> read_rom(U16 address)
    FUNCTION<U8> read_ram(U16 address)
    PROCEDURE write_ram(U16 address, U8 u8)
    PROCEDURE insert(STRING _file_path)
    PROCEDURE save_ram()
    PROCEDURE load_ram()
```

---

## `MBC`

The MBC class extends cartridge ROM and RAM by utilising memory banks. The MBC facilitates the switching of banks for cartridges with more than 32KB of memory. Different MBCs inherit from a base MBC but have different implementations.

```pseudo
BASE CLASS MBC:
    BOOL is_ram_enabled
    BOOL is_rom_bank_mode
    INT type
    INT rom_bank
    INT ram_bank
    INT total_rom_banks

    CONSTRUCTOR MBC(INT _type)
    VIRTUAL PROCEDURE write(U16 address, U8 u8)
    PROCEDURE modify_rom_bank(U16 address, REFERENCE<U8> temporary_rom_bank)
```

---

### `MBC1`

MBC1 is a specific implementation of the MBC that supports up to 2MB of ROM and 32KB of RAM. It controls ROM and RAM bank switching as well as bank mode switching between RAM and ROM banks.

```pseudo
CLASS MBC1 INHERITS MBC:
    CONSTRUCTOR MBC1()
    PROCEDURE write(U16 address, U8 u8) OVERRIDE
    PROCEDURE modify_rom_bank(U16 address, REFERENCE<U8> temporary_rom_bank)
```

---

### `MBC2`

MBC2 is a simpler MBC that supports up to 256KB of ROM and 512 bytes of internal RAM. It primarily handles ROM bank switching and doesn't have a bank switching mode like MBC1.

```pseudo
CLASS MBC2 INHERITS MBC:
    CONSTRUCTOR MBC2()
    PROCEDURE write(U16 address, U8 u8) OVERRIDE
```

---

## `Joypad`

The Joypad class handles input from the Gameboy's game key and controller binds and manages the joypad state.

```pseudo
CLASS Joypad:
    REFERENCE<CPU> cpu
    U8 joypad
    U8 joypad_button_states

    CONSTRUCTOR Joypad(REFERENCE<CPU> _cpu)
    PROCEDURE reset()
    FUNCTION<U8> read()
    PROCEDURE press_button(int button_bit)
    PROCEDURE check_key_pressed(EVENT event, JSON key_binds)
    PROCEDURE check_key_released(EVENT event, JSON key_binds)
    PROCEDURE check_controller_button_pressed(EVENT event, JSON controller_binds)
    PROCEDURE check_controller_button_released(EVENT event, JSON controller_binds)
    PROCEDURE check_controller_dpad_pressed(EVENT event, JSON controller_binds)
```

---

## `LCD`

The LCD class handles the visual output of the Gameboy, including rendering pixels, scaling, and managing frame buffers. It features a retro mode which displays grid lines and blends previous and current frames, simulating a ghosting effect.
```pseudo
CLASS LCD:
    ENUM FrameBlendStrength : WEAK, MEDIUM, STRONG
    INT scale_factor
    INT full_screen_scale_factor
    INT gap_width
    INT gap_height
    INT black_bar_width
    INT black_bar_height
    VECTOR position
    INT width
    INT height
    INT frame_blend_strength
    BOOL is_retro_mode_enabled
    INT number_of_frame_buffers
    VECTOR<ARRAY<U8, 23040>> frame_buffers
    VERTEX_ARRAY frame_buffer_vertices

    CONSTRUCTOR LCD()
    PROCEDURE reset()
    PROCEDURE transfer_pixel(INT x, INT y, U8 pixel)
    PROCEDURE update_frame_buffers()
    PROCEDURE display(REFERENCE<WINDOW> window, ARRAY<COLOUR, 4>> palette)
    PROCEDURE scale_pixel(INT pixel_index, VECTOR pixel_position, COLOUR pixel_color)
    FUNCTION<COLOUR> blend_pixel(INT location, ARRAY<COLOUR, 4>> palette)
```

---

## `MMU`

The MMU handles the Gameboy's memory map, acting as an intermediary for communication between the CPU and other components like PPU, Cartridge, and Joypad.

```pseudo
CLASS MMU:
    REFERENCE<CPU> cpu
    REFERENCE<PPU> ppu
    REFERENCE<Cartridge> cartridge
    REFERENCE<Joypad> joypad
    REFERENCE<Timer> timer
    BOOL is_bootstrap_enabled
    STRING exe_path
    ARRAY<U8> bootstrap
    UNIQUE_POINTER<U8[]> work_ram
    UNIQUE_POINTER<U8[]> high_ram

    CONSTRUCTOR MMU(REFERENCE<CPU> _cpu, REFERENCE<PPU> _ppu, REFERENCE<Cartridge> _cartridge, REFERENCE<Joypad> _joypad, REFERENCE<Timer> _timer, STRING _exe_path)
    PROCEDURE reset()
    PROCEDURE load_bootstrap()
    FUNCTION<U8> read_u8(U16 address)
    FUNCTION<U16> read_u16(U16 address)
    PROCEDURE write_u8(U16 address, U8 u8)
    PROCEDURE write_u16(U16 address, U16 u16)
    PROCEDURE perform_dma_transfer(U8 encoded_source_address)
```

---

## `PPU`

The PPU is responsible for rendering graphics to the screen. It handles background, window, and sprite rendering as well as LCD control.

```pseudo
CLASS PPU:
    ENUM Mode : H_BLANK, V_BLANK, OAM_SEARCH, PIXEL_TRANSFER
    REFERENCE<MMU> mmu
    REFERENCE<LCD> lcd
    REFERENCE<CPU> cpu
    INT ticks
    INT mode
    INT h_blank_interval
    INT v_blank_interval
    INT oam_search_interval
    INT pixel_transfer_interval
    BOOL is_lcd_enabled
    BOOL is_window_tile_map_1_selected
    BOOL is_window_enabled
    BOOL is_unsigned_background_tileset_selected
    BOOL is_background_tile_map_1_selected
    BOOL are_objects_8x16
    BOOL is_background_enabled
    BOOL are_objects_enabled
    BOOL is_scanline_comparison_enabled
    BOOL is_oam_search_stat_interrupt_enabled
    BOOL is_v_blank_stat_interrupt_enabled
    BOOL is_h_blank_stat_interrupt_enabled
    BOOL is_scanline_comparison_equal
    U8 scroll_x
    U8 scroll_y
    U8 scanline_y
    U8 scanline_y_comparison
    U8 window_x
    U8 window_y
    U8 background_palette
    U8 object_palette_0
    U8 object_palette_1
    UNIQUE_POINTER<U8[]> video_ram
    UNIQUE_POINTER<U8[]> oam
    UNIQUE_POINTER<U8[]> current_scanline_buffer

    CONSTRUCTOR PPU(REFERENCE<MMU> _mmu, REFERENCE<LCD> _lcd, REFERENCE<CPU> _cpu)
    PROCEDURE reset()
    FUNCTION<U8> read(U16 address)
    PROCEDURE write(U16 address, U8 u8)
    PROCEDURE set_lcd_control(U8 u8)
    FUNCTION<U8> get_lcd_control()
    PROCEDURE set_lcd_status(U8 u8)
    FUNCTION<U8> get_lcd_status()
    PROCEDURE run(INT last_instruction_ticks)
    PROCEDURE run_hblank()
    PROCEDURE run_vblank()
    PROCEDURE run_oam_search()
    PROCEDURE run_pixel_transfer()
    PROCEDURE check_lcd_y_comparison()
    PROCEDURE render_scanline()
    PROCEDURE render_background()
    PROCEDURE render_window()
    PROCEDURE render_objects()
    FUNCTION<U16> get_tile_location(U8 tile_index)
    FUNCTION<U8> get_color_from_id(U8 color_id, U8 palette)
```

---

## `Timer`

The Timer class emulates the Gameboy's internal timer and divider registers. These are two timer registers with slightly different behaviour and are used for things such as clocks, timers and other time-bound events.

```pseudo
CLASS Timer:
    REFERENCE<CPU> cpu
    REFERENCE<MMU> mmu
    U8 divider
    U8 counter
    U8 modulo
    U8 clock_select
    BOOL is_counter_enabled
    INT counter_period
    INT counter_ticks
    INT divider_period
    INT divider_ticks

    CONSTRUCTOR Timer(REFERENCE<MMU> _mmu, REFERENCE<CPU> _cpu)
    PROCEDURE reset()
    FUNCTION<U8> read(U16 address)
    PROCEDURE write(U16 address, U8 u8)
    PROCEDURE set_counter_period()
    PROCEDURE run(INT last_instruction_ticks)
    PROCEDURE update_divider()
    PROCEDURE update_counter()
```

---

## `StateManager`

The `StateManager` class maintaining a stack of states, allowing for state transitions and maintaining a stack of states. The state at the top of the stack is the current state being processed.

```pseudo
CLASS StateManager:
    ENUM StateType : MAIN_MENU, INSERT_ROM, EMULATION, PAUSED, SETTINGS, GENERAL_SETTINGS, DISPLAY_SETTINGS,
                     PALETTES, MODIFY_PALETTE, COLOR_PICKER, CONTROLS, INPUT_SELECT, REMAP_BINDS

    REFERENCE<Gameboy> gameboy
    State current_state
    STACK<State> current_state_stack

    CONSTRUCTOR StateManager(REFERENCE<Gameboy> _gameboy)
    PROCEDURE reset()
    PROCEDURE run()
    PROCEDURE fade_into_or_out_of_state(BOOL is_fading_in)
    PROCEDURE run_current_state()
    PROCEDURE switch_current_state()
    PROCEDURE push_new_state(INT state_type)
```

---

## `State`

The `State` class represents a general application state with UI elements and logic for user interactions, rendering, and transitions.

```pseudo
BASE CLASS State:
    ENUM StateType : MAIN_MENU, INSERT_ROM, EMULATION, PAUSED, SETTINGS, GENERAL_SETTINGS, DISPLAY_SETTINGS,
                     PALETTES, MODIFY_PALETTE, COLOR_PICKER, CONTROLS, INPUT_SELECT, REMAP_BINDS

    REFERENCE<Gameboy> gameboy
    BOOL is_screen_fading
    INT transition_state
    BOOL has_entered_new_state
    BOOL is_running
    STRING name
    VECTOR<UNIQUE_POINTER<UIElement>> ui_elements
    sf::Color screen_fade_color
    DOUBLE screen_fade_opacity
    INT menu_type
    INT cursor
    INT length_between_ui_elements
    INT ui_element_width
    INT ui_element_font_size
    INT ui_element_height
    INT scrollbar_height
    INT scrollbar_width
    FLOAT scrollbar_size
    Utilities::Vector scrollbar_start
    Utilities::Vector scrollbar_end
    Utilities::Vector scrollbar_position
    Utilities::Vector ui_element_offset

    CONSTRUCTOR State(REFERENCE<Gameboy> _gameboy, STRING _name)
    VIRTUAL PROCEDURE reset()
    VIRTUAL PROCEDURE configure_ui_elements()
    VIRTUAL PROCEDURE configure_ui_element_parameters()
    PROCEDURE configure_scrollbar()
    PROCEDURE handle_events()
    PROCEDURE exit_app()
    PROCEDURE jump_to_last_button()
    VIRTUAL PROCEDURE handle_keyboard_events()
    VIRTUAL PROCEDURE handle_controller_events()
    VIRTUAL PROCEDURE perform_logic()
    PROCEDURE animate_ui_elements()
    VIRTUAL PROCEDURE render()
    VIRTUAL PROCEDURE display_heading()
    PROCEDURE display_fps()
    PROCEDURE display_ui_elements()
    PROCEDURE display_scrollbar()
    VIRTUAL PROCEDURE display_miscellaneous()
    VIRTUAL PROCEDURE update_cursor(INT scroll_direction)
    PROCEDURE update_cursor_after_ui_element_reconfiguration()
    PROCEDURE scroll_ui_elements(INT direction)
    PROCEDURE update_scrollbar()
    PROCEDURE update_selected_ui_element()
    PROCEDURE enter_new_state(INT state_type)
    PROCEDURE fade_in()
    PROCEDURE fade_out()
    PROCEDURE apply_screen_fade()
    PROCEDURE return_to_previous_state()
```

---

## `MainMenuState`

The `MainMenuState` class represents the main menu of the application.

```pseudo
CLASS MainMenuState INHERITS State:
    CONSTRUCTOR MainMenuState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    OVERRIDE PROCEDURE configure_ui_element_parameters()
    OVERRIDE PROCEDURE display_heading()
```

---

## `RemapBindsState`

The `RemapBindsState` class handles the remapping of input binds, including keyboard and controller inputs.

```pseudo
CLASS RemapBindsState INHERITS State:
    CONSTRUCTOR RemapBindsState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    OVERRIDE PROCEDURE handle_keyboard_events()
    OVERRIDE PROCEDURE handle_controller_events()
    FUNCTION<BOOL> does_already_contain_bind(INT bind_code)
    OVERRIDE PROCEDURE perform_logic()
    PROCEDURE update_binds()
```

---

## `PausedState`

The `PausedState` class represents the pause menu when the user pauses emulation, allowing users to resume, navigate to homne or settings.

```pseudo
CLASS PausedState INHERITS State:
    CONSTRUCTOR PausedState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    PROCEDURE return_to_home()
```

---

## `PalettesState`

The `PalettesState` class allows users to view and customise palettes used for the emulator's theme.

```pseudo
CLASS PalettesState INHERITS State:
    CONSTRUCTOR PalettesState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    OVERRIDE PROCEDURE perform_logic()
    PROCEDURE update_selected_palette()
    PROCEDURE customise_palette_segment()
    PROCEDURE add_palette()
```

---

## `ModifyPaletteState`

The `ModifyPaletteState` class enables users to modify specific palettes.

```pseudo
CLASS ModifyPaletteState INHERITS State:
    CONSTRUCTOR ModifyPaletteState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    OVERRIDE PROCEDURE configure_ui_element_parameters()
    OVERRIDE PROCEDURE display_miscellaneous()
    PROCEDURE display_palette()
    PROCEDURE enter_color_picker_state()
```

---

## `SettingsState`

The `SettingsState` class represents the settings menu of the application, allowing users to configure various options.

```pseudo
CLASS SettingsState INHERITS State:
    CONSTRUCTOR SettingsState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
```

---

## `InsertRomState`

The `InsertRomState` class allows users to load a ROM into the emulator.

```pseudo
CLASS InsertRomState INHERITS State:
    CONSTRUCTOR InsertRomState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    PROCEDURE insert_rom()
    OVERRIDE PROCEDURE perform_logic()
    PROCEDURE update_roms()
    FUNCTION<BOOL> is_a_gb_file(STRING rom_path)
```

---

## `InputSelectState`

The `InputSelectState` class allows the user to select the input type for remapping controls.

```pseudo
CLASS InputSelectState INHERITS State:
    CONSTRUCTOR InputSelectState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    PROCEDURE enter_remap_binds_state(STRING input_type)
```

---

## `GeneralSettingsState`

The `GeneralSettingsState` class provides options to modify general emulator settings, including emulation speed and bootstrap behaviour.

```pseudo
CLASS GeneralSettingsState INHERITS State:
    VECTOR<STRING> emulation_speed_options
    VECTOR<STRING> bootstrap_options
    UNORDERED_MAP<STRING, INT> emulation_speed_to_value
    UNORDERED_MAP<INT, STRING> value_to_emulation_speed

    CONSTRUCTOR GeneralSettingsState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    OVERRIDE PROCEDURE perform_logic()
    PROCEDURE update_general_settings()
    PROCEDURE restore_default_settings()
```

---

## `EmulationState`

The `EmulationState` class handles the active emulation, including user interactions, rendering, and logic for emulating the Gameboy.

```pseudo
CLASS EmulationState INHERITS State:
    CONSTRUCTOR EmulationState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    OVERRIDE PROCEDURE handle_keyboard_events()
    OVERRIDE PROCEDURE handle_controller_events()
    OVERRIDE PROCEDURE perform_logic()
    OVERRIDE PROCEDURE render()
    PROCEDURE display_full_screen_black_bars()
```

---

## `DisplaySettingsState`

The `DisplaySettingsState` class allows users to modify settings related to the emulator's display, such as FPS, scaling, and retro mode.

```pseudo
CLASS DisplaySettingsState INHERITS State:
    VECTOR<STRING> target_fps_options
    VECTOR<STRING> display_fps_options
    VECTOR<STRING> scale_factor_options
    VECTOR<STRING> retro_mode_options
    VECTOR<STRING> frame_blend_strength_options

    UNORDERED_MAP<STRING, INT> target_fps_to_value
    UNORDERED_MAP<STRING, INT> scale_factor_to_value
    UNORDERED_MAP<STRING, INT> frame_blend_strength_to_value

    UNORDERED_MAP<INT, STRING> value_to_target_fps
    UNORDERED_MAP<INT, STRING> value_to_scale_factor
    UNORDERED_MAP<INT, STRING> value_to_frame_blend_strength

    CONSTRUCTOR DisplaySettingsState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    OVERRIDE PROCEDURE perform_logic()
    PROCEDURE update_display_settings()
```

---

## `ControlsState`

The `ControlsState` class allows the user to select the control type for remapping controls.
```pseudo
CLASS ControlsState INHERITS State:
    CONSTRUCTOR ControlsState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    PROCEDURE enter_input_select_state(STRING control_type)
```

---

## `ColorPickerState`

The `ColorPickerState` class enables users to pick colours for palettes and other settings.

```pseudo
CLASS ColorPickerState INHERITS State:
    ENUM DIRECTION : LEFT, RIGHT, UP
    sf::Color current_color

    CONSTRUCTOR ColorPickerState(REFERENCE<Gameboy> _gameboy)
    OVERRIDE PROCEDURE configure_ui_elements()
    OVERRIDE PROCEDURE handle_keyboard_events()
    OVERRIDE PROCEDURE handle_controller_events()
    OVERRIDE PROCEDURE update_cursor(INT direction)
    OVERRIDE PROCEDURE perform_logic()
    PROCEDURE update_current_color()
    OVERRIDE PROCEDURE display_miscellaneous()
    PROCEDURE display_current_color()
    PROCEDURE save()
    PROCEDURE discard()
```

---
