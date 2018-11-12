
/*
 * Copyright (C) Igor Sysoev
 */


#ifndef _NGX_CYCLE_H_INCLUDED_
#define _NGX_CYCLE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef NGX_CYCLE_POOL_SIZE
#define NGX_CYCLE_POOL_SIZE     16384
#endif


#define NGX_DEBUG_POINTS_STOP   1
#define NGX_DEBUG_POINTS_ABORT  2


typedef struct ngx_shm_zone_s  ngx_shm_zone_t;

typedef ngx_int_t (*ngx_shm_zone_init_pt) (ngx_shm_zone_t *zone, void *data);

struct ngx_shm_zone_s {
    void                     *data;
    ngx_shm_t                 shm;
    ngx_shm_zone_init_pt      init;
    void                     *tag;
};


struct ngx_cycle_s {
    //保存着所有模块配置项的结构体指针
    void                  ****conf_ctx;
    ngx_pool_t               *pool;

    //临时log对象，待errlog生成之后会替换为new_log
    ngx_log_t                *log;
    ngx_log_t                 new_log; //最后使用的log，初始化完成后会覆盖log成员

    // ngx_connect_t的指针所构成的数组，用于保存事件模块的链接。
    ngx_connection_t        **files; 
    ngx_connection_t         *free_connections;     //可用连接池
    ngx_uint_t                free_connection_n;    //可用连接池的数量

    ngx_array_t               listening;            //保存着ngx_listening_t成员，表示监听端口及相关的参数
    ngx_array_t               pathes;               //用到的路径
    ngx_list_t                open_files;           //表示nginx已经打开的所有文件，框架向open_files链表添加这些文件，nginx在ngx_init_cycle中打开这些文件
    ngx_list_t                shared_memory;        //ngx_shm_zone_t的容器，每个元素表示一块内存

    ngx_uint_t                connection_n;         //connections的总数
    ngx_uint_t                files_n;              //files的总数

    ngx_connection_t         *connections;          //当前进程中的所有连接对象
    ngx_event_t              *read_events;          //当前进程中的所有读事件对象
    ngx_event_t              *write_events;         //当前进程中的所有写事件对象

    ngx_cycle_t              *old_cycle;            //用于引用上一个ngx_cycle_t的成员

    ngx_str_t                 conf_file;            // 配置文件相对于安装目录的路径名称
    ngx_str_t                 conf_param;           // 启动时的命令行参数
    ngx_str_t                 conf_prefix;          // ngx配置文件所在的目录的路径
    ngx_str_t                 prefix;               // ngx安装目录的路径
    ngx_str_t                 lock_file;            // 用于进程间同步的文件锁名称
    ngx_str_t                 hostname;             // 使用gethostname得到的主机名称
};


typedef struct {
     ngx_flag_t               daemon;
     ngx_flag_t               master;

     ngx_msec_t               timer_resolution;

     ngx_int_t                worker_processes;
     ngx_int_t                debug_points;

     ngx_int_t                rlimit_nofile;
     ngx_int_t                rlimit_sigpending;
     off_t                    rlimit_core;

     int                      priority;

     ngx_uint_t               cpu_affinity_n;
     u_long                  *cpu_affinity;

     char                    *username;
     ngx_uid_t                user;
     ngx_gid_t                group;

     ngx_str_t                working_directory;
     ngx_str_t                lock_file;

     ngx_str_t                pid;
     ngx_str_t                oldpid;

     ngx_array_t              env;
     char                   **environment;

#if (NGX_THREADS)
     ngx_int_t                worker_threads;
     size_t                   thread_stack_size;
#endif

} ngx_core_conf_t;


typedef struct {
     ngx_pool_t              *pool;   /* pcre's malloc() pool */
} ngx_core_tls_t;


#define ngx_is_init_cycle(cycle)  (cycle->conf_ctx == NULL)


ngx_cycle_t *ngx_init_cycle(ngx_cycle_t *old_cycle);
ngx_int_t ngx_create_pidfile(ngx_str_t *name, ngx_log_t *log);
void ngx_delete_pidfile(ngx_cycle_t *cycle);
ngx_int_t ngx_signal_process(ngx_cycle_t *cycle, char *sig);
void ngx_reopen_files(ngx_cycle_t *cycle, ngx_uid_t user);
char **ngx_set_environment(ngx_cycle_t *cycle, ngx_uint_t *last);
ngx_pid_t ngx_exec_new_binary(ngx_cycle_t *cycle, char *const *argv);
u_long ngx_get_cpu_affinity(ngx_uint_t n);
ngx_shm_zone_t *ngx_shared_memory_add(ngx_conf_t *cf, ngx_str_t *name,
    size_t size, void *tag);


extern volatile ngx_cycle_t  *ngx_cycle;
extern ngx_array_t            ngx_old_cycles;
extern ngx_module_t           ngx_core_module;
extern ngx_uint_t             ngx_test_config;
extern ngx_uint_t             ngx_quiet_mode;
#if (NGX_THREADS)
extern ngx_tls_key_t          ngx_core_tls_key;
#endif


#endif /* _NGX_CYCLE_H_INCLUDED_ */
