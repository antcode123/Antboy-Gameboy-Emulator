cmake_minimum_required(VERSION 3.10)

project(antboy VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -mwindows")
set(SRC_DIR "Source/")
set(HW_DIR "${SRC_DIR}Hardware/")
set(UTILS_DIR "${SRC_DIR}Utilities/")
set(OP_DIR "${SRC_DIR}Opcodes/")
set(EXT_DIR "External Libraries/")
set(UI_DIR "${SRC_DIR}UI/")
set(UI_STATES_DIR "${UI_DIR}/States/")
set(UI_ELMT_DIR "${UI_DIR}/UI Elements/")

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    )

    FetchContent_MakeAvailable(SFML)

    set(SOURCES
    ${SRC_DIR}main.cpp
    ${SRC_DIR}gameboy.cpp
    ${UI_DIR}state_manager.cpp
    ${UI_ELMT_DIR}ui_element.cpp
    ${UI_ELMT_DIR}button.cpp
    ${UI_ELMT_DIR}arrow_selector.cpp
    ${UI_ELMT_DIR}palette.cpp
    ${UI_ELMT_DIR}color_picker.cpp
    ${UI_ELMT_DIR}bind_remapper.cpp
    ${UI_STATES_DIR}state.cpp
    ${UI_STATES_DIR}main_menu_state.cpp
    ${UI_STATES_DIR}settings_state.cpp
    ${UI_STATES_DIR}general_settings_state.cpp
    ${UI_STATES_DIR}display_settings_state.cpp
    ${UI_STATES_DIR}palettes_state.cpp
    ${UI_STATES_DIR}modify_palette_state.cpp
    ${UI_STATES_DIR}color_picker_state.cpp
    ${UI_STATES_DIR}controls_state.cpp
    ${UI_STATES_DIR}input_select_state.cpp
    ${UI_STATES_DIR}remap_binds_state.cpp
    ${UI_STATES_DIR}insert_rom_state.cpp
    ${UI_STATES_DIR}emulation_state.cpp
    ${UI_STATES_DIR}paused_state.cpp
    ${UTILS_DIR}vector.cpp
    ${UTILS_DIR}renderer.cpp
    ${UTILS_DIR}misc.cpp
    ${OP_DIR}alu_opcodes.cpp
    ${OP_DIR}misc_opcodes.cpp
    ${OP_DIR}jump_opcodes.cpp
    ${OP_DIR}func_opcodes.cpp
    ${OP_DIR}bitmanip_opcodes.cpp
    ${OP_DIR}load_opcodes.cpp
    ${OP_DIR}opcodes_helper.cpp
    ${HW_DIR}cpu.cpp
    ${HW_DIR}mmu.cpp
    ${HW_DIR}ppu.cpp
    ${HW_DIR}timer.cpp
    ${HW_DIR}joypad.cpp
    ${HW_DIR}cartridge.cpp
    ${HW_DIR}mbc.cpp
    ${HW_DIR}lcd.cpp
    )

    add_executable(antboy ${SOURCES})

    target_link_libraries(antboy
    sfml-graphics
    sfml-window
    sfml-system
    )

    target_include_directories(antboy PRIVATE "${SRC_DIR}/")
    set_target_properties(antboy PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../")
    target_compile_options(antboy PRIVATE "-O3")