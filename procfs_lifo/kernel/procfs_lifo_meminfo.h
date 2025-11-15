#include <linux/types.h>

#define GET_STATS _IOR('a', 'a', struct procfs_lifo_meminfo_t *)
#define CLEAR_LIFO _IOW('a', 'b', struct procfs_lifo_meminfo_t *)

#define PROCFS_LIFO_CHUNK_SIZ 1024

struct procfs_lifo_meminfo_t {
    __u64 total_alloc_bytes;
    __u64 len;
    __u64 nodes;
};
