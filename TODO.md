## 阶段 1 · 最小可运行内核

### 1.1 Multiboot 引导
- [x] 编写 `boot/boot.asm`
- [x] 放置 Multiboot 头（magic `0x1BADB002`，flags，校验和）
- [x] 定义 `_start` 为全局符号
- [x] `_start` 中先 `cli` 关中断
- [x] 在 `.bss` 中预留内核栈（≥16KB）
- [x] 设置 `esp` 指向栈顶
- [x] 把 `eax`（magic）与 `ebx`（info 指针）压栈
- [x] 调用 `kernel_main`
- [x] 返回后 `hlt` 死循环

### 1.2 基础输出
- [x] `kernel/drivers/vga.c`：VGA 文本模式初始化
- [x] 实现屏幕清屏
- [x] 实现单字符输出
- [x] 实现换行、回车、退格、Tab
- [x] 实现滚动
- [x] 实现基本颜色控制
- [x] `kernel/drivers/serial.c`：串口初始化（COM1，0x3F8）
- [x] 实现 `serial_putc`
- [x] 实现 `serial_puts`
- [ ] 实现串口中断接收（可选，后期）
- [x] 实现统一 `kprintf`，同时输出到 VGA 与串口
- [x] 支持 `%d %u %x %p %s %c %%`

### 1.3 GDT
- [x] 定义 GDT 条目结构
- [x] 建立平坦模型：内核代码段、内核数据段
- [x] 预留用户代码段、用户数据段、TSS 段位置
- [x] 编写 `gdt.asm` 中的 `gdt_flush`
- [x] C 侧构造 GDT 并调用 `gdt_flush`
- [x] 用远跳转刷新 `cs`
- [x] 重载 `ds/es/fs/gs/ss`
- [x] 验证 GDT 生效后内核继续运行

### 1.4 IDT 与异常
- [x] 定义 IDT 条目结构
- [x] 编写 `isr.asm`：0–31 号异常存根
- [x] 对推送错误码的异常单独处理
- [x] 统一 `isr_common`：`pusha`、保存段寄存器、切内核数据段
- [x] 调用 C 侧 `isr_handler`
- [x] 恢复寄存器并 `iretd`
- [x] C 侧注册默认处理函数
- [x] 实现 `#DE` 除零异常报告
- [x] 实现 `#PF` 页错误报告（读取 `CR2`）
- [x] 实现 `#GP` 通用保护错误报告
- [x] 实现 `#DF` 双重错误报告
- [x] 编写 `idt.asm` 中的 `idt_flush`
- [x] 加载 IDT 并验证异常可被捕获

### 1.5 阶段验证
- [x] QEMU 启动后 VGA 与串口同时输出欢迎信息
- [x] 手动触发除零异常，能打印信息且不三重故障
- [x] `git commit` 阶段成果

---

## 阶段 2 · 内存管理

### 2.1 物理内存探测
- [x] 解析 Multiboot info 中的内存映射
- [ ] 若无内存映射，改用 BIOS `INT 0x15, EAX=0xE820`
- [x] 把内存映射规范化为统一结构
- [x] 打印可用物理内存布局

### 2.2 物理页帧分配器
- [x] 定义页大小常量（4KB）
- [x] 实现位图分配器
- [x] 计算位图所需空间
- [x] 把位图放在内核保留区
- [x] 标记内核已用页
- [x] 标记 Multiboot 模块占用页
- [x] 标记保留区
- [x] 实现 `pmm_alloc_page`
- [x] 实现 `pmm_free_page`
- [ ] 实现连续多页分配
- [x] 实现分配统计（已用/空闲）

### 2.3 分页机制
- [ ] 定义页目录与页表结构
- [ ] 分配页目录（4KB 对齐）
- [ ] 建立恒等映射（至少前 16MB）
- [ ] 设置页目录项属性（present、rw、user）
- [ ] 编写 `paging.asm` 中的 `load_page_directory`
- [ ] 编写 `paging.asm` 中的 `enable_paging`
- [ ] 开启 `CR0.PG`
- [ ] 开启 `CR0.WP` 写保护
- [ ] 验证开启分页后内核正常运行
- [ ] 实现 `map_page(virt, phys, flags)`
- [ ] 实现 `unmap_page(virt)`
- [ ] 实现按需分配页表
- [ ] 实现 `get_physical(virt)`

### 2.4 页错误处理
- [ ] 在 `#PF` 处理中读 `CR2`
- [ ] 读取错误码判断原因（present / write / user / reserved）
- [ ] 区分缺页与保护违规
- [ ] 缺页时尝试按需分配物理页
- [ ] 保护违规时向进程发送 SIGSEGV（后期）
- [ ] 记录出错地址与进程信息

### 2.5 内核堆
- [ ] 确定堆所在虚拟地址区间
- [ ] 预留若干页作为初始堆
- [ ] 实现简单空闲链表分配器
- [ ] 实现 `kmalloc`
- [ ] 实现 `kfree`
- [ ] 实现 `kcalloc`
- [ ] 实现 `krealloc`
- [ ] 处理对齐要求
- [ ] 堆不足时自动扩展（连续映射更多页）
- [ ] 加入调试信息：分配大小、调用位置
- [ ] 加入基本越界检测（可选）

### 2.6 阶段验证
- [ ] `kmalloc` / `kfree` 压测通过
- [ ] 故意访问未映射地址能触发 `#PF` 并被捕获
- [ ] 分页后 GDT/IDT/栈仍正常
- [ ] `git commit`

---

## 阶段 3 · 中断、时钟与抢占式调度

### 3.1 PIC 与 IRQ
- [ ] 重新映射 PIC，把 IRQ 移到 0x20–0x2F
- [ ] 屏蔽所有 IRQ，逐个开启
- [ ] 实现 IRQ 统一存根（`irq.asm`）
- [ ] C 侧 IRQ 分发表
- [ ] 实现 `irq_register_handler`
- [ ] 每个 IRQ 处理完发送 EOI

### 3.2 PIT 时钟
- [ ] 配置 PIT 通道 0
- [ ] 设定频率（建议 100Hz 起步，后期可调）
- [ ] 注册 IRQ0 处理程序
- [ ] 维护全局 tick 计数
- [ ] 实现 `get_ticks`
- [ ] 实现毫秒级延时（基于 tick）

### 3.3 任务结构
- [ ] 定义 `struct task`
- [ ] 字段含：`pid`、`state`、`esp`、`cr3`、`page_dir`、`kernel_stack`、`name`、`priority`、`ticks_left`
- [ ] 预留用户态字段：`uid/gid/euid/egid`、文件描述符表、信号表
- [ ] 定义任务状态枚举：`RUNNING / READY / BLOCKED / ZOMBIE`
- [ ] 实现任务创建：分配内核栈、设置初始栈帧
- [ ] 让新任务首次被调度时能正确 `ret` 到入口函数
- [ ] 实现任务销毁与资源回收

### 3.4 上下文切换
- [ ] 编写 `switch.asm` 中的 `switch_context`
- [ ] 保存 `pusha` 与 `pushfd`
- [ ] 保存旧 `esp` 到旧任务结构
- [ ] 加载新任务 `esp`
- [ ] 恢复 `popfd` 与 `popa`
- [ ] `ret` 到新任务上次中断点
- [ ] 切换时同步切换 `CR3`（进程独立地址空间后启用）
- [ ] 验证两个内核线程可来回切换

### 3.5 调度器
- [ ] 维护就绪队列
- [ ] 实现 `scheduler_init`
- [ ] 实现 `scheduler_add_task`
- [ ] 实现 `scheduler_remove_task`
- [ ] 实现 `scheduler_tick`
- [ ] 实现时间片轮转
- [ ] 在时钟中断中调用 `scheduler_tick`
- [ ] 时间片耗尽时切到下一任务
- [ ] 实现 `schedule` 主动让出
- [ ] 实现空转任务（idle task）
- [ ] 无就绪任务时切到 idle 并 `hlt`

### 3.6 睡眠与阻塞
- [ ] 实现 `sleep(ms)`
- [ ] 睡眠队列按到期 tick 排序
- [ ] 每个 tick 检查到期任务
- [ ] 到期任务移入就绪队列
- [ ] 实现 `task_block`
- [ ] 实现 `task_unblock`
- [ ] 为后续信号量/互斥量预留接口

### 3.7 阶段验证
- [ ] 两个内核线程交替打印，验证抢占生效
- [ ] `sleep` 精度符合预期
- [ ] 长时间运行无死锁、无栈溢出
- [ ] `git commit`

---

## 阶段 4 · 系统调用与用户态

### 4.1 TSS 与特权级
- [ ] 定义 TSS 结构
- [ ] 在 GDT 中添加 TSS 描述符
- [ ] 编写 `tss.asm` 中的 `tss_flush`
- [ ] 设置 `ss0` 与 `esp0` 指向内核栈
- [ ] 在任务切换时更新 `esp0`
- [ ] 在 GDT 中添加 Ring 3 代码段与数据段
- [ ] 验证 `iret` 能进入 Ring 3

### 4.2 系统调用入口
- [ ] 编写 `syscall_entry.asm`
- [ ] 约定：`eax` = 调用号，`ebx/ecx/edx/esi/edi` = 参数
- [ ] 保存现场（`pusha` + 段寄存器）
- [ ] 切换到内核数据段与内核栈
- [ ] 调用 C 侧 `syscall_handler`
- [ ] 返回值放回 `eax`
- [ ] 恢复现场并 `iretd`
- [ ] 在 IDT 中注册 `0x80`
- [ ] 对非法调用号返回 `-ENOSYS`

### 4.3 系统调用实现
- [ ] `sys_exit`
- [ ] `sys_write`
- [ ] `sys_read`
- [ ] `sys_open`
- [ ] `sys_close`
- [ ] `sys_lseek`
- [ ] `sys_brk`
- [ ] `sys_getpid`
- [ ] `sys_getuid / sys_geteuid / sys_getgid / sys_getegid`
- [ ] `sys_fork`（先做简化版）
- [ ] `sys_execve`（先做简化版）
- [ ] `sys_waitpid`
- [ ] `sys_kill`（预留）
- [ ] 所有指针参数做用户地址空间校验
- [ ] 统一错误码返回风格（负 errno）

### 4.4 用户地址空间
- [ ] 设计用户地址布局（代码、数据、堆、栈、共享库区）
- [ ] 内核空间与用户空间分界（如 3GB）
- [ ] 用户页目录只映射用户空间与必要内核入口
- [ ] 每个进程独立页目录
- [ ] 切换进程时切换 `CR3`
- [ ] 用户栈按需增长

### 4.5 ELF 加载器
- [ ] 校验 ELF32 magic
- [ ] 解析 ELF 头与程序头表
- [ ] 遍历 `PT_LOAD` 段
- [ ] 按段权限映射到用户地址空间
- [ ] 处理 `.bss`（零填充）
- [ ] 设置用户栈
- [ ] 把 `argc/argv/envp` 压入用户栈
- [ ] 构造 `iret` 帧进入 Ring 3
- [ ] 加载失败时返回错误并回收资源

### 4.6 第一个用户程序
- [ ] 写一个最小用户程序（输出后 `exit`）
- [ ] 用 `i686-elf-gcc` 编译为静态 ELF
- [ ] 链接到固定用户地址
- [ ] 用内核加载器运行
- [ ] 验证输出与正常退出
- [ ] `git commit`

---

## 阶段 5 · 文件系统与设备驱动

### 5.1 VFS 抽象层
- [ ] 定义 `struct inode`
- [ ] 定义 `struct dentry`
- [ ] 定义 `struct file`
- [ ] 定义 `struct super_block`
- [ ] 定义 `inode_ops`（lookup、create、unlink、read、write、truncate、mkdir、readdir）
- [ ] 定义 `file_ops`（open、close、read、write、lseek、ioctl）
- [ ] 实现路径解析
- [ ] 实现挂载点
- [ ] 实现根文件系统注册
- [ ] 实现文件描述符表（每进程）
- [ ] 实现 `open/close/read/write/lseek/stat/fstat`
- [ ] 实现 `opendir/readdir/closedir`
- [ ] 实现 `mkdir/rmdir/unlink/rename`

### 5.2 具体文件系统（先 FAT16）
- [ ] 解析引导扇区（BPB）
- [ ] 解析 FAT 表
- [ ] 解析根目录区
- [ ] 实现短文件名读取
- [ ] 实现长文件名读取（可选）
- [ ] 实现文件读取
- [ ] 实现文件写入
- [ ] 实现文件创建与删除
- [ ] 实现目录创建与删除
- [ ] 挂载为根文件系统
- [ ] 后续扩展 Ext2（可选）

### 5.3 块设备层
- [ ] 定义块设备接口（读扇区、写扇区）
- [ ] 实现块设备注册
- [ ] 实现缓冲区缓存（可选，后期）
- [ ] 统一扇区大小（512B）

### 5.4 ATA PIO 驱动
- [ ] 探测主/从通道
- [ ] 识别磁盘型号
- [ ] 实现 `ata_read_sectors`
- [ ] 实现 `ata_write_sectors`
- [ ] 处理 28 位 LBA
- [ ] 处理忙等待与状态轮询
- [ ] 加入超时保护
- [ ] 注册为块设备

### 5.5 键盘驱动
- [ ] 注册 IRQ1
- [ ] 读取 `0x60` 扫描码
- [ ] 处理按下与释放
- [ ] 实现扫描码到 ASCII 映射
- [ ] 处理 Shift/Ctrl/Alt
- [ ] 处理 CapsLock/NumLock
- [ ] 环形缓冲区保存按键
- [ ] 抽象为字符设备 `/dev/kbd`
- [ ] 支持阻塞读取（无键时阻塞）

### 5.6 其他字符设备
- [ ] `/dev/tty`：屏幕输出
- [ ] `/dev/null`
- [ ] `/dev/zero`
- [ ] `/dev/console`：键盘 + 屏幕

### 5.7 阶段验证
- [ ] 用户程序能通过 POSIX 接口读写文件
- [ ] 能从键盘读取输入
- [ ] 文件系统能持久化到磁盘镜像
- [ ] `git commit`

---

## 阶段 6 · POSIX libc 移植

### 6.1 选择与准备 libc
- [ ] 选定 Newlib（推荐）或 uClibc
- [ ] 配置为 `i686-elf` 目标
- [ ] 关闭不需要的功能（线程、locale 等先关）
- [ ] 构建静态库 `libc.a`

### 6.2 底层存根
- [ ] `_write` → `sys_write`
- [ ] `_read` → `sys_read`
- [ ] `_open` → `sys_open`
- [ ] `_close` → `sys_close`
- [ ] `_lseek` → `sys_lseek`
- [ ] `_fstat` → `sys_fstat`
- [ ] `_stat` → `sys_stat`
- [ ] `_isatty` → 判断是否为 tty
- [ ] `_sbrk` → 扩展用户堆
- [ ] `_exit` → `sys_exit`
- [ ] `_kill` → `sys_kill`
- [ ] `_getpid` → `sys_getpid`
- [ ] `_fork` → `sys_fork`
- [ ] `_execve` → `sys_execve`
- [ ] `_waitpid` → `sys_waitpid`
- [ ] `_times` → 返回进程时间
- [ ] `_gettimeofday` → 基于 tick 或 RTC
- [ ] `_link`、`_unlink`、`_mkdir` 等 → 对应 VFS
- [ ] 未实现项先返回 `-ENOSYS` 并记录

### 6.3 用户态运行时
- [ ] 编写 `crt0.asm`（NASM）
- [ ] 设置用户栈
- [ ] 调用 `__libc_init_array`（构造器）
- [ ] 调用 `main(argc, argv, envp)`
- [ ] 调用 `exit`
- [ ] 支持 `atexit` 注册的清理函数
- [ ] 支持析构器
- [ ] 与 ELF 加载器约定初始栈布局

### 6.4 验证
- [ ] 用户程序能 `#include <stdio.h>`
- [ ] `printf` 格式化输出正确
- [ ] `malloc/free` 工作正常
- [ ] `fopen/fread/fwrite/fclose` 正常
- [ ] `str*`、`mem*` 系列正常
- [ ] `errno` 正确设置
- [ ] `perror` 正常
- [ ] `git commit`

---

## 阶段 7 · 模块化与多用户

### 7.1 内核模块框架
- [ ] 定义模块接口结构
- [ ] 定义 `module_init` / `module_exit`
- [ ] 定义模块元信息（作者、许可、依赖）
- [ ] 内核维护模块链表
- [ ] 实现 `insmod` 系统调用
- [ ] 实现 `rmmod` 系统调用
- [ ] 实现 `lsmod` 系统调用
- [ ] 模块可注册字符设备
- [ ] 模块可注册块设备
- [ ] 模块可注册文件系统
- [ ] 模块可注册系统调用（可选）
- [ ] 模块加载时做符号解析
- [ ] 导出内核符号表

### 7.2 用户与权限
- [ ] 在任务结构中加入 `uid/gid/euid/egid`
- [ ] 加入 `suid/sgid` 支持
- [ ] 实现 `setuid/setgid/seteuid/setegid`
- [ ] 实现 `getuid/getgid/geteuid/getegid`
- [ ] 实现 `chmod/chown`
- [ ] 文件 inode 保存权限位与所有者
- [ ] VFS 在 `open/read/write/exec` 时检查权限
- [ ] 定义 root（uid=0）特权
- [ ] 实现 `umask`
- [ ] 实现登录与用户数据库（`/etc/passwd`、`/etc/group`）

### 7.3 进程隔离
- [ ] 每进程独立页目录
- [ ] 切换进程时刷新 `CR3`
- [ ] 用户态不可访问内核页（U/S 位）
- [ ] 内核态不可访问用户页（除显式拷贝）
- [ ] 实现 `copy_from_user` / `copy_to_user`
- [ ] 实现 `fork` 的写时复制（COW）
- [ ] COW 页错误处理
- [ ] 实现 `execve` 替换地址空间
- [ ] 实现 `waitpid` 回收子进程
- [ ] 实现僵尸进程与孤儿进程处理
- [ ] 进程组与会话（可选，后期）

### 7.4 安全加固
- [ ] 所有系统调用参数校验
- [ ] 栈保护（用户栈 guard page）
- [ ] 内核栈溢出检测
- [ ] 空指针页不映射
- [ ] 只读段写保护
- [ ] 审计日志（可选）

### 7.5 阶段验证
- [ ] 两个不同 uid 用户无法互读私有文件
- [ ] 加载/卸载模块不影响系统稳定
- [ ] COW 下 `fork` 性能可接受
- [ ] `git commit`

---

## 阶段 8 · 完善与自举

### 8.1 信号
- [ ] 定义信号表（SIGKILL、SIGSEGV、SIGINT、SIGTERM 等）
- [ ] 实现 `signal`
- [ ] 实现 `sigaction`
- [ ] 实现 `kill`
- [ ] 实现 `raise`
- [ ] 用户态信号处理函数入口与栈帧
- [ ] `sigreturn` 恢复上下文
- [ ] 不可捕获信号处理（SIGKILL、SIGSTOP）

### 8.2 进程间通信
- [ ] 实现匿名管道 `pipe`
- [ ] 实现命名管道 `mkfifo`
- [ ] 实现 Unix 域套接字
- [ ] 实现共享内存 `shmget/shmat`
- [ ] 实现消息队列（可选）
- [ ] 实现信号量（可选）

### 8.3 同步原语
- [ ] 内核自旋锁
- [ ] 内核互斥量
- [ ] 内核信号量
- [ ] 用户态 `pthread_mutex`
- [ ] 用户态 `pthread_cond`
- [ ] 用户态 `pthread_create / join`

### 8.4 更多驱动
- [ ] RTC 实时时钟
- [ ] PCI 总线枚举
- [ ] VESA / VBE 图形模式
- [ ] 简单帧缓冲
- [ ] 鼠标驱动
- [ ] 网卡驱动（NE2000 / RTL8139）
- [ ] 网络协议栈（IP / UDP / TCP，可选）
- [ ] 声卡驱动（可选）

### 8.5 Shell 与用户工具
- [ ] 编写 Shell
- [ ] 支持命令执行
- [ ] 支持管道 `|`
- [ ] 支持重定向 `> < >>`
- [ ] 支持后台执行 `&`
- [ ] 支持环境变量
- [ ] 支持 `cd`、`pwd`、`export`
- [ ] 移植 `ls`
- [ ] 移植 `cat`
- [ ] 移植 `echo`
- [ ] 移植 `cp`
- [ ] 移植 `mv`
- [ ] 移植 `rm`
- [ ] 移植 `mkdir`
- [ ] 移植 `ps`
- [ ] 移植 `kill`
- [ ] 移植 `grep`（可选）
- [ ] 移植 `sed`（可选）
- [ ] 移植 `awk`（可选）
- [ ] 移植 `vi` 或 `nano`（可选）

### 8.6 系统初始化
- [ ] 内核启动流程整理：`kernel_main` 分阶段
- [ ] 挂载根文件系统
- [ ] 启动第一个用户进程 `init`
- [ ] `init` 读取 `/etc/inittab` 或 `/etc/rc`
- [ ] 启动 getty 与 login
- [ ] 提供多虚拟终端（可选）
- [ ] 实现关机与重启系统调用

### 8.7 自举
- [ ] 在系统内运行 GCC
- [ ] 在系统内运行 binutils
- [ ] 在系统内运行 NASM
- [ ] 用系统自身编译一个用户程序
- [ ] 用系统自身编译内核
- [ ] 完整自举成功

### 8.8 质量与文档
- [ ] 内核单元测试（可选框架）
- [ ] 系统调用回归测试
- [ ] 文件系统一致性测试
- [ ] 调度压力测试
- [ ] 内存泄漏检测
- [ ] 编写开发者文档
- [ ] 编写用户手册
- [ ] 编写贡献指南
- [ ] 建立 CI（可选，用 QEMU 自动跑）

---

## 附：每阶段通用检查项

- [ ] 本阶段所有 `.asm` 用 NASM 汇编通过
- [ ] 本阶段所有 `.c` 用 `i686-elf-gcc` 编译无警告
- [ ] 链接无未定义符号
- [ ] QEMU 中稳定运行 10 分钟以上
- [ ] 无三重故障重启
- [ ] 串口日志清晰可读
- [ ] 用 GDB 验证过关键路径
- [ ] 代码已提交 Git
- [ ] 更新本 TODO 清单勾选状态

---

## 附：常用调试手段清单

- [ ] `qemu -d int,cpu_reset -no-reboot` 查看中断与异常
- [ ] `qemu -s -S` + `gdb` 远程断点
- [ ] `i686-elf-objdump -d` 反汇编定位
- [ ] `i686-elf-readelf -a` 检查 ELF 结构
- [ ] `grub-mkrescue` 失败时检查 `grub.cfg` 路径
- [ ] 串口输出用 `-serial stdio` 捕获
- [ ] 三重故障时用 `-d int` 定位最后异常

---

按此清单逐项推进，每完成一个阶段再做下一阶段。祝项目顺利。
