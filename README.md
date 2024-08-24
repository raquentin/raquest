# 🏓 raquest (raq)

Raquest is a batteries-excluded command-line HTTP client built Insomnia and Postman are trash. No monthly subscriptions or two minute ads to send raquests, just run `raq -f <query_name>.raq` and move on with your life.

## Features

### v1 todo-list
- [x] support all request types
- [x] support json in files
- [x] support for headers
- [ ] cookies
- [x] good parser errors
- [ ] lsp
- [x] tests of any sort
- [ ] docs
- [ ] logging
- [ ] plugin for asserting that certain requests recieve certain responses
- [x] ci

## Development

### Dependencies
- cmake 3.22+
- g++ 14

### Running Tests
```bash
    mkdir -p build \
    cd build \
    cmake .. \
    cmake --build . \
    ctest
```

### Clangd Setup
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```
