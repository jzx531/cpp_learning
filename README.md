# scan ports

```Mermaid
flowchart TD
    Start([程序启动]) --> CheckArgs{参数少于4个？}
    CheckArgs -- 是 --> PrintUsage[打印用法说明] --> CleanExit1[清理资源并退出]
    CheckArgs -- 否 --> InitNet[初始化 Winsock 和线程属性]
    
    InitNet --> ParseLoop{解析命令行参数}
    
    subgraph 参数解析循环
        ParseLoop -- 选项-p --> ParsePort[解析端口列表存入数组]
        ParsePort --> CheckPortErr{端口合法？}
        CheckPortErr -- 否 --> PortErr[报错退出]
        CheckPortErr -- 是 --> ParseLoop
        
        ParseLoop -- 选项-t --> SetThread[设置全局线程数变量]
        SetThread --> ParseLoop
        
        ParseLoop -- 选项-d --> SetDebug[设置调试标志为1]
        SetDebug --> ParseLoop
        
        ParseLoop -- 无更多参数 --> GetIPs
    end
    
    GetIPs[获取起止IP字符串] --> ConvIP[转换IP为整数]
    ConvIP --> CallScan[调用 scan 主函数]
    
    subgraph 扫描主控逻辑 scan
        CallScan --> LoopIP{遍历 IP 范围}
        LoopIP -- 结束 --> WaitEnd[等待3秒后返回]
        
        LoopIP -- 有 IP --> BatchPrep[计算本批次数量]
        BatchPrep --> AllocData[分配数据数组和线程句柄]
        
        subgraph 批次内循环
            AllocData --> FillData[填充 IP 和端口数据]
            FillData --> CheckPortPool{端口池用完？}
            CheckPortPool -- 是 --> ResetPort[重置端口索引并切换IP]
            CheckPortPool -- 否 --> CreateTh[创建 pthread 线程]
            
            CreateTh --> JoinTh[立即 join 等待线程结束]
            JoinTh --> SleepShort[休眠 10ms]
            SleepShort --> NextInBatch{本批次结束？}
            NextInBatch -- 否 --> FillData
        end
        
        NextInBatch -- 是 --> SleepLong[休眠 1000ms]
        SleepLong --> LoopIP
    end
    
    subgraph 线程工作逻辑 threadscan
        CreateTh -.-> ThreadStart[线程启动]
        ThreadStart --> ExtractData[提取 IP 和端口]
        ExtractData --> DebugOut{是否调试模式？}
        DebugOut -- 是 --> PrintDebug[打印调试信息]
        DebugOut -- 否 --> CreateSock
        
        PrintDebug --> CreateSock[创建 TCP Socket]
        CreateSock --> SetupAddr[填充目标地址结构]
        SetupAddr --> ConnectTry[尝试 connect 连接]
        
        ConnectTry --> CheckConn{连接成功？}
        CheckConn -- 是 --> PrintOpen[打印端口开放]
        CheckConn -- 否 --> CloseSock
        
        PrintOpen --> CloseSock[关闭 Socket]
        CloseSock --> ThreadExit[线程退出]
    end
    
    WaitEnd --> CleanExit2[清理 Winsock 并正常退出]
    
    style Start fill:#f9f,stroke:#333
    style CleanExit1 fill:#ff9999,stroke:#333
    style CleanExit2 fill:#99ff99,stroke:#333
    style PrintOpen fill:#ffff99,stroke:#333
    style JoinTh fill:#ffcccc,stroke:#333,stroke-dasharray: 5 5
```

运行结果如下：
```shell
./output/main.exe  -p 21,22,23,80,443,8080 -t 256 192.168.1.1 192.168.1.254
192.168.1.11     8080 Open
192.168.1.17     23 Open
192.168.1.20     8080 Open
192.168.1.42     23 Open
192.168.1.64     23 Open
192.168.1.101    23 Open
192.168.1.253    23 Open
192.168.1.254    80 Open
192.168.1.254    443 Open
```
