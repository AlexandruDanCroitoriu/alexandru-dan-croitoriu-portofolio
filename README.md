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