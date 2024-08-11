# 🏓 raquest (raq)

Raquest is a batteries-excluded command-line HTTP client built Insomnia and Postman are trash. No monthly subscriptions or two minute ads to send raquests, just run `raq -f <query_name>.raq` and move on with your life.

## Features

### Todo
- [x] support all request types
- [x] support json in files
- [ ] support for headers
- [ ] cookies
- [ ] good parser errors
- [ ] lsp
- [ ] tests of any sort
- [ ] docs
- [ ] plugin for asserting that certain requests recieve certain responses

## Development

### Dependencies
- cmake 3.22+
- g++ 14
- libcurl
- cli11 v2.4.2 (it's in /include)

### Clangd Setup
```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```
