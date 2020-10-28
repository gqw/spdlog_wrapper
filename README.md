# spdlog_wrapper

#### 介绍
`spdlog`简单封装

#### 安装教程

1.  mkdir build && cd build
3.  cmake ..
4.  make

#### 使用说明

1.  直接使用`logger.h`头文件
2.  代码示例：
   ```CPP
    #include "logger.h"

    int main() {
        if (!logger::get().init("logs/test.log")) {
            return 1;
        }
        
        STM_DEBUG() << "STM_DEBUG" << 1;
        PRINT_WARN("PRINT_WARN, %d", 1);
        LOG_INFO("LOG_INFO {}", 1);

        logger::get().set_level(spdlog::level::info);
        STM_DEBUG() << "STM_DEBUG " << 2;
        PRINT_WARN("PRINT_WARN, %d", 2);
        LOG_INFO("LOG_INFO {}", 2);

        // call before spdlog static variables destroy
        logger::get().shutdown();
        return 0;
    }
   ```

#### 文档介绍

[说明](./doc/index.md)

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request

