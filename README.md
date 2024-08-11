# 🏓 raquest (raq)

Raquest is a minimalist command-line HTTP client on top of `libcurl`. It's a foss `curl++`-like response to closed-source bloatware like Insomnia and Postman being absolute shite. No monthly subscriptions or two minute ads to access your requests, just save them in `.raq` files, run `raq -f <query_name>.raq`, and move on with your life.

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
