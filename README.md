# ARMv8 Emulator and Assembler

This repository contains a C-based emulator and assembler for ARMv8 instructions. It includes modules for parsing, processing, and emulating ARMv8 assembly code, as well as utilities for binary manipulation and symbol table management.

## Project Structure

### Key Files
- **`emulate.c`**: Core emulation logic for ARMv8 instructions.
- **`assemble.c`**: Assembler for converting ARMv8 assembly to binary.
- **`parse_file.c`**: Parses input assembly files.
- **`process_instr.c`**: Processes individual ARMv8 instructions.
- **`binary.c`**: Utilities for binary operations and conversions.
- **`dispatch.c`**: Dispatches instructions to their respective handlers.
- **`utils.c`**: General utility functions.
- **`globals.c`**: Global variables and constants.
- **`sym_tab.c`**: Symbol table management.

### Headers
- **`*.h`**: Header files for the corresponding `.c` files, defining interfaces and shared structures.

## Building the Project

1. Ensure you have `gcc` and `make` installed.
2. Run the following command in the `src` directory:
   ```bash
   make
   ```
   This will compile the project and generate the executable.

## Usage

### Emulator
To emulate ARMv8 instructions:
```bash
./emulator <input_file>
```

### Assembler
To assemble ARMv8 assembly code:
```bash
./assembler <input_file>
```

## Dependencies
- Standard C libraries.
- `make` for building.

## Contributing
Contributions are welcome! Please ensure your changes are well-documented and tested.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.