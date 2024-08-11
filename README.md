# 🏓 raquest (raq)

Raquest is a command-line HTTP client on top of `libcurl`. I ragequit using postman and insomnia and could not find a `curl`-premium-like solution that wasn't shite. No watching a 2min add to access your requests, just save them in `.raq` files, run `raq -f <query_name>.raq`, and move on with your life.

## Development

### dependencies
- cmake
- g++ 14
- c++ 20
- libcurl

### Clangd LSP setup
```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```
