# alexandru-dan-croitoriu-portofolio



### Building and Running
Debug:
```bash
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -j$(nproc)
make run
```

Release:
```bash
mkdir -p build/release
cd build/release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make -j$(nproc)
make run
```

Tailwind
```bash
cd static/stylus/tailwind/
npm run watch

# for release, compile the minified css file
npm run minify
```