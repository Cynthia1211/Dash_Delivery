# Dash_Delivery
Course Project of PROG33921 Game Engineering Principles

## Building from Source
### macOS (Intel)
Ensure you have `raylib` installed. Run the following command in your terminal to compile the game:

```bash
clang++ main.cpp -std=c++17 -I/usr/local/include -L/usr/local/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -o game