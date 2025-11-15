# Dropbear SSH Server for TrimUI Smart Pro

A graphical SSH server application for the TrimUI Smart Pro handheld gaming device, providing remote shell access with an intuitive on-screen interface.

## Features

- **Full SSH Server**: Runs Dropbear SSH daemon with persistent host keys
- **Visual Interface**: Real-time display of network addresses and server logs
- **Network Discovery**: Automatically detects and displays IPv4 addresses (WiFi, Ethernet, USB)
- **Controller Support**: Navigate and exit using gamepad controls
- **Persistent Host Keys**: Generates and reuses RSA host keys to prevent connection warnings
- **Live Logging**: Streams Dropbear logs directly to the on-screen display

## Requirements

- TrimUI Smart Pro device
- Dropbear SSH binaries:
  - `dropbear` (SSH server daemon)
  - `dropbearkey` (host key generator)
  - `dbclient` (SSH client, optional)
  - `scp` (secure copy, optional)

## Installation

1. Clone the repository with submodules:
   ```bash
   git clone --recursive git@github.com:dimitrovs/trimui-smart-pro-build-system.git
   ```
   
   Or if you've already cloned the repository, initialize the submodule:
   ```bash
   git submodule update --init --recursive
   ```

2. Build the application:
   ```bash
   make
   ```

3. The compiled application will be in `build/Dropbear/`

4. Copy the entire `build/Dropbear/` folder to your TrimUI Smart Pro's Apps directory

## Usage

### Starting the Server

1. Launch the Dropbear app from your TrimUI Smart Pro menu
2. The application will:
   - Generate an RSA host key on first run (may take a moment)
   - Start the Dropbear SSH server
   - Display all available network IP addresses
   - Show live server logs

### Connecting via SSH

Once the application is running and shows your device's IP address:

```bash
ssh root@<device-ip-address>
```

Default Dropbear configuration typically uses port 22.

### Exiting the Application

Press **START + SELECT** simultaneously on your controller to exit.

## Project Structure

```
.
├── src/
│   ├── main.cpp              # Application entry point
│   ├── Application.h/cpp     # Main application orchestrator
│   ├── DropbearManager.h/cpp # SSH server lifecycle management
│   ├── NetworkManager.h/cpp  # Network interface discovery
│   ├── Renderer.h/cpp        # SDL rendering logic
│   ├── PathHelper.h/cpp      # Path resolution utilities
│   ├── Color.h               # Color definitions
│   └── Constants.h           # Application constants
├── res/
│   ├── arial.ttf             # Font for UI text
│   └── icon.png              # Application icon
├── tests/
│   ├── test_main.cpp         # Test entry point
│   ├── test_PathHelper.cpp   # Path resolution tests
│   ├── test_NetworkManager.cpp # Network tests
│   └── test_Color.cpp        # Color utilities tests
├── Makefile                  # Build configuration
└── README.md                 # This file
```

## Architecture

The application follows modern C++ best practices with clear separation of concerns:

- **Application**: Coordinates all components and manages SDL lifecycle
- **DropbearManager**: Handles Dropbear process management, host key generation, and log streaming
- **NetworkManager**: Discovers and formats network interface information
- **Renderer**: Handles all SDL2/TTF rendering operations
- **PathHelper**: Resolves paths to bundled executables and data files

## Building from Source

### Prerequisites

**For Application Build:**
- Cross-compilation toolchain: `aarch64-linux-gnu-g++`
- SDL2 development libraries (cross-compiled for target)
- SDL2_ttf development libraries (cross-compiled for target)
- Build environment with SDL_CFLAGS, SDL_LDFLAGS, SDL_LIBS set

**For Tests (host machine):**
- g++ with C++11 support
- Standard Linux development tools

### Build Commands

```bash
# Build the application (requires TrimUI build environment)
make

# Build and run tests (can be done independently)
make test

# Clean build artifacts
make clean
```

**Note**: The main application must be built within the TrimUI Smart Pro build system environment which provides the necessary SDL cross-compilation settings. Tests can be built and run on any Linux system with g++.

### Environment Variables

The Makefile uses the following environment variables (set by the build system):

- `SDL_CFLAGS`: SDL2 compiler flags
- `SDL_LDFLAGS`: SDL2 linker flags
- `SDL_LIBS`: SDL2 libraries

## Configuration

### Dropbear Options

The application starts Dropbear with the following flags:

- `-E`: Log to stderr (captured for display)
- `-F`: Don't daemonize (run in foreground)
- `-r <keypath>`: Use specific RSA host key

### Host Key Location

The RSA host key is stored in the same directory as the application executable:
- Filename: `dropbear_rsa_host_key`
- Type: RSA
- Generated on first run if not present

## Troubleshooting

### No IP Address Displayed

- Ensure WiFi or Ethernet is connected
- The app refreshes IP addresses every 2 seconds
- Check network interface is up and configured

### Dropbear Fails to Start

- Verify `dropbear` and `dropbearkey` binaries are present in the app directory
- Check the on-screen logs for specific error messages
- Ensure binaries have execute permissions

### SSH Connection Refused

- Confirm the Dropbear app is running
- Verify you're using the correct IP address shown on screen
- Check firewall settings on the device

### Host Key Warnings

- The app generates a persistent host key to avoid this
- If you see warnings, the host key may have been deleted or corrupted
- Remove the `dropbear_rsa_host_key` file and restart to regenerate

## Development

### Running Tests

```bash
make test
./build/test_runner
```

### Adding New Features

1. Create new header/implementation files in `src/`
2. Add source files to `Makefile` SRC variable
3. Update tests in `tests/` directory
4. Rebuild with `make clean && make`

### Code Style

- Modern C++11/14 features
- RAII for resource management
- Single Responsibility Principle
- Const-correctness
- Header guards (`#pragma once`)

## Performance

- **Frame Rate**: 60 FPS target (16ms per frame)
- **Log Buffer**: 300 lines maximum
- **IP Refresh**: Every 2 seconds
- **Memory**: Minimal allocations, bounded buffers

## Security Considerations

- Dropbear runs with the same privileges as the application
- Default configuration allows root login
- Consider password or key-based authentication setup
- Host key persists between sessions (stored unencrypted)

## License

This application integrates Dropbear SSH, which is licensed under MIT-style license.
See Dropbear documentation for full licensing information.

## Credits

- **Dropbear SSH**: Matt Johnston
- **SDL2**: Sam Lantinga and SDL contributors
- **TrimUI Smart Pro**: TrimUI

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

## Changelog

### Version 1.0.0
- Initial release
- Dropbear SSH server integration
- Visual network interface display
- Persistent host key management
- Controller-based navigation
- Real-time log streaming
