# ngx_http_hello_world

Simple example on how to implement a nginx C module to obtain: Method, IP, User-Agent, Cookies, and Post/Get Variables

# Install

1. Download nginx
```
wget https://nginx.org/download/nginx-1.18.1.tar.gz
tar -xzf nginx-1.18.1.tar.gz
export NGINX_PATH=$(pwd)/nginx-1.18.1/
```
2. Compile the module
```
git clone https://github.com/bnchdan/ngx_http_hello_world
cd ngx_http_hello_world
./build.sh
```
3. Compile the module
```
git clone https://github.com/bnchdan/ngx_http_hello_world
cd ngx_http_hello_world
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
![image](https://github.com/bnchdan/ngx_http_hello_world/assets/30780133/4203c7b4-5b8a-42f1-870b-54a5823cd596)


![image](https://github.com/bnchdan/ngx_http_hello_world/assets/30780133/3a26b56f-6a7d-4a21-a2f5-85faee5a5211)
