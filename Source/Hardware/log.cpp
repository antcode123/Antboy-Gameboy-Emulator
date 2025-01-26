
    // This logger is formatted to be read by the gameboy doctor application
    // The logs for my CPU are compared line by line to pre-processed logs of a CPU accurate emulator
    // If a log doesn't match, then the gameboy doctor will highlight where it doesn't match
    // It will also provide the faulty opcode which resulted in the mismatch

    // std::ostringstream logStream;
    // logStream << std::hex << std::uppercase << std::setfill('0');
    // logStream << "A:" << std::setw(2) << static_cast<int>(registers.A) << " "
    //           << "F:" << std::setw(2) << static_cast<int>(registers.F) << " "
    //           << "B:" << std::setw(2) << static_cast<int>(registers.B) << " "
    //           << "C:" << std::setw(2) << static_cast<int>(registers.C) << " "
    //           << "D:" << std::setw(2) << static_cast<int>(registers.D) << " "
    //           << "E:" << std::setw(2) << static_cast<int>(registers.E) << " "
    //           << "H:" << std::setw(2) << static_cast<int>(registers.H) << " "
    //           << "L:" << std::setw(2) << static_cast<int>(registers.L) << " "
    //           << "SP:" << std::setw(4) << static_cast<int>(stack_pointer) << " "
    //           << "PC:" << std::setw(4) << static_cast<int>(program_counter) << " "
    //           << "PCMEM:"
    //           << std::setw(2) << static_cast<int>(mmu.read_u8(program_counter)) << ","
    //           << std::setw(2) << static_cast<int>(mmu.read_u8(program_counter + 1)) << ","
    //           << std::setw(2) << static_cast<int>(mmu.read_u8(program_counter + 2)) << ","
    //           << std::setw(2) << static_cast<int>(mmu.read_u8(program_counter + 3));

    // logs.push_back(logStream.str());


    // std::ofstream logFile("logs.txt");

    // for (const std::string& log : gameboy.cpu.logs) {
    //     logFile << log << std::endl;
    // }

    // logFile.close();

    // std::cout << "Logs saved to logs.txt successfully." << std::endl;