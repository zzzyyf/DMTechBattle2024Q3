# DMTechBattle2024Q3

## 题目

1. 生成一个 600 字节长的字符串，内容为小写 'a'-'z' 随机
2. 开始计时 t1
3. 生成 1000 万个 [3, 600] 字节长度随机的字符串（可以多线程，但是要求每个线程只保留1个当前字符串）
    - 字符串内容：头两个字节用**十六进制**记录字符串长度，剩下的内容从步骤 1 字符串的第 3 个字节开始截取
    - 如生成长度为 1 的字符串，则先写入十六进制的字符串长度对应的字符 01，再写入步骤 1 生成字符串的第 3 个字节
4. 将所有生成的字符串写入到**一个**数据文件中
    - 要求每条记录都必须完整写入数据文件，不能出现只写入部分的情况，包括`\0`也要写入
    - 可以先写到内存缓冲区（缓冲区总大小不超过 8 MB），再异步写入数据文件
5. 完成所有写入后，关闭文件，记录时间 t2
6. 输出 t2 - t1 的值

## 分析
- （8 线程 ~2400ms，16 线程 ~1900ms）先来个最简单的，将字符串直接生成到各线程缓冲区中，再统一传递给一个 IO 线程写文件
    - 交由 IO 线程写文件可以让 IO 与 CPU 解耦
    - 需要控制缓冲区总大小，可以预先分配好每个线程的 buffer 总大小，然后使用双 buffer，一份给 IO 线程写，一份自己再生成数据
    - 不能只写入部分数据，缓冲区必须只存放完整的字符串数据
    - 先不使用文件映射
