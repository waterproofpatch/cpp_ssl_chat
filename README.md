# SslChat

```bash
cd SslChat
mkdir build
cd build
```

## Build

OSX

```bash
cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl -DOPENSSL_LIBRARIES=/usr/local/opt/openssl/lib ..
```

Linux

```bash
cmake -DOPENSSL_ROOT_DIR=/usr/include/openssl -DOPENSSL_LIBRARIES=/usr/lib/ssl ..
```
