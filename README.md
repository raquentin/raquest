# 🏓 raquest

Raquest is a batteries-excluded command-line HTTP client. No monthly subscriptions or two minute ads to send requests, just run `raq <query>.raq` and move on with your life.

## Features

### `.raq` files

Requests are stored written in `'.raq'` files and resemble `curl` commands in bash scripts. The files consist of sections defining the request, its headers, its json payload, and assertions, which assert certain attributes of the repsonse. View these sections in action in the [examples](/examples) or conceptually below:

#### [request]
```
[request]
POST https://jsonplaceholder.typicode.com/posts
```
The `[request]` section is generally found at the top of the file and defines the HTTP method and the target url.

#### [headers]
```
[headers]
Authorization: Bearer exampletoken12345
Content-Type: application/json
Custom-Header: custom_value
```
The `[headers]` section defines key-value header pairs.

#### [body]
```
[body]
{
  "title": "foo",
  "age": 30,
  "isActive": true,
  "description": "here is the description"
}
```
The `[body]` section contains the request payload; its type is inferred from the `Content-Type` header in the `[headers]` section.

#### [assertions]
```
[assertions]
status: 201, 303
json_field: title ^foo$
json_field: age 30
json_field: isActive true
json_field: description here is the description
```
`[assertions]` is an optional section that allows you to assert certain attributes of your request's response.

## Todos for v1
- [x] support all request types
- [x] support json in files
- [x] support for headers
- [ ] cookies
- [ ] good parser errors
- [ ] lsp
- [ ] support all std body types
- [ ] tests of any sort
- [ ] docs
- [ ] logging
- [x] assertions
- [ ] ci

## Development

### Dependencies
- cmake 3.24+
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
