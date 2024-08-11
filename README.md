# 🏓 raquest (raq)

Raquest is a minimalist command-line HTTP client on top of `libcurl`. It's a foss `curl++`-like response to closed-source bloatware like Insomnia and Postman being absolute shite. No monthly subscriptions or two minute ads to access your requests, just save them in `.raq` files, run `raq -f <query_name>.raq`, and move on with your life.

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

### A batteries-excluded design

Raquest intentionally has no version control, response formatting, or search. Use `git`, `jq`, and `rg` for that.

## Development

### Dependencies
- cmake 3.22+
- g++ 14
- libcurl
- cli11 v2.4.2 (it's in /include)

### Clangd LSP setup
```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```
