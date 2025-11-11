#define GET_STATS _IOR('a', 'a', struct procfs_lifo_meminfo_t *)
#define CLEAR_LIFO _IOW('a', 'b', struct procfs_lifo_meminfo_t *)

#define PROCFS_LIFO_CHUNK_SIZ 1024

struct procfs_lifo_meminfo_t {
    uint64_t total_alloc_bytes;
    uint64_t len;
    uint16_t nodes;
};
