# ngx_http_hello_world

Simple example on how to implement a nginx C module to obtain: Method, IP, User-Agent, Cookies and Post/Get Variable

# Install

1. Download nginx
```
wget https://nginx.org/download/nginx-1.16.1.tar.gz
tar -xzf nginx-1.16.1.tar.gz
export NGINX_PATH=$(pwd)/nginx-1.16.1/
```
2. Compile the module
```
git clone https://github.com/simon987/ngx_http_js_challenge_module
cd ngx_http_js_challenge_module
./build.sh
```
3. Compile the module
```
git clone https://github.com/simon987/ngx_http_js_challenge_module
cd ngx_http_js_challenge_module
./build.sh
```
# Configuration
```
load_module /my_path/ngx_http_hello_world_module.so;
...
http {
  ...
  server{
    ...
    location /hello_world {
        hello_world;
    }
    ...
  }
  ...
}
...
```

go to url/hello_world
