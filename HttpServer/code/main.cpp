#include <unistd.h>
#include "./server/webserver.h"

int main() {
    /* 守护进程 后台运行 */
    //daemon(1, 0); 

    WebServer server(
        8888, 3, 60000, true,             /* 端口 ET模式 timeoutMs 优雅退出  */
        3306, "zs", "20000117zs", "WebServerDB", /* Mysql配置 */
        1, 1, true, 0, 1024);             /* 连接池数量 线程池数量 日志开关 日志等级 日志异步队列容量 */
   
    server.Start();
} 