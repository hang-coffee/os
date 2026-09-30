# OS - 操作系统

## 构建指导

需要安装的依赖项：`nasm`、`gcc`、`binutils`、`qemu-system-i386`、`grub-pc-bin`、`grub-commons`、`xorriso`、`mtools`。

前三样工具用于编译操作系统内核。如果要制作可启动的`iso`文件（`make iso`），则后四者是必须的。如果要方便地启动模拟软件（`make run`），则`qemu-system-i386`是必须的。

其中，`gcc`与`binutils`需要交叉编译。`gcc`需要支持`gnu11`及以上版本。可以从源代码编译它们：

编译`binutils`：
```bash
tar xf binutils-x.x.x.tar.xz
cd binutils-x.x.x/
mkdir build && cd build
../configure --target=i686-elf --prefix=/usr/local/cross --disable-nls --disable-werror
make -j$(nproc)
sudo make install
```

编译`gcc`：
```bash
tar xf gcc-x.x.x.tar.xz
tar xf gmp-x.x.x.tar.xz
tar xf mpc-x.x.x.tar.xz
tar xf mpfr-x.x.x.tar.xz
mv gmp-x.x.x gcc-x.x.x/gmp
mv mpc-x.x.x gcc-x.x.x/mpc
mv mpfr-x.x.x mpfr-x.x.x/mpfr
cd gcc-x.x.x/
mkdir build && cd build
../configure --target=i686-elf --prefix=/usr/local/cross --disable-nls --enable-languages=c --without-headers
make -j$(nproc) all-gcc all-target-libgcc
sudo make install-gcc install-target-gcc
```

`make` 来编译，`make run`来运行。

## 许可证

GNU GPL 2.0，请见LICENSE
