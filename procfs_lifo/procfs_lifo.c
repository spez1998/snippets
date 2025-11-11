#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/err.h>
#include <linux/minmax.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include "procfs_lifo_meminfo.h"

static int __init procfs_lifo_init(void);
static void __exit procfs_lifo_exit(void);

static int procfs_lifo_open(struct inode *inode, struct file *file);
static int procfs_lifo_release(struct inode *inode, struct file *file);
static ssize_t procfs_lifo_read(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t procfs_lifo_write(struct file *file, const char *buf, size_t len, loff_t *off);
static long procfs_lifo_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct proc_dir_entry *parent;
static struct proc_ops fops = {
    .proc_open = procfs_lifo_open,
    .proc_release = procfs_lifo_release,
    .proc_read = procfs_lifo_read,
    .proc_write = procfs_lifo_write,
    .proc_ioctl = procfs_lifo_ioctl
};

/* LIFO data structure */

// Linked list of chunks for dynamic data storage
struct node {
    char data[PROCFS_LIFO_CHUNK_SIZ];
    struct list_head list;
};

LIST_HEAD(head_node);

struct procfs_lifo_meminfo_t procfs_lifo_meminfo = {0};

// Mutex to handle concurrent writers
struct mutex procfs_lifo_mutex;
DEFINE_MUTEX(procfs_lifo_mutex);

/* LIFO data structure */

static int procfs_lifo_open(struct inode *inode, struct file *file) {
    pr_info("procfs_lifo file opened\n");
    return 0;
}

static int procfs_lifo_release(struct inode *inode, struct file *file) {
    pr_info("procfs_lifo file released\n");
    return 0;
}

static ssize_t procfs_lifo_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    struct node *node;
    uint64_t copied = 0, relative_start = 0, relative_end = 0, start = 0, used = 0;
    uint16_t first_node = 0, last_node = 0;
    int i = 0;

    if (len > procfs_lifo_meminfo.len) {
	len = procfs_lifo_meminfo.len;
    }

    start = procfs_lifo_meminfo.len - len;
    first_node = start / PROCFS_LIFO_CHUNK_SIZ;
    relative_start = start % PROCFS_LIFO_CHUNK_SIZ;
    last_node = procfs_lifo_meminfo.len / PROCFS_LIFO_CHUNK_SIZ;
    relative_end = procfs_lifo_meminfo.len % PROCFS_LIFO_CHUNK_SIZ;

    char *buff = (char *)kzalloc(len, GFP_KERNEL);
    if (!buff) {
	pr_err("procfs_lifo couldn't allocate space for user read()\n");
	return -ENOMEM;
    }

    list_for_each_entry(node, &head_node, list) {
    	if (i == first_node) {
            if (first_node == last_node) {
		memcpy(buff, node->data + relative_start, relative_end - relative_start);
		copied += relative_end - relative_start;
		break;
	    } else {
	        memcpy(buff, node->data + relative_start, PROCFS_LIFO_CHUNK_SIZ - relative_start);
	        copied += PROCFS_LIFO_CHUNK_SIZ - relative_start;
	    }
	}
	     
	if (i > first_node) {
	    if (list_is_last(&node->list, &head_node)) {
                used = PROCFS_LIFO_CHUNK_SIZ - (procfs_lifo_meminfo.total_alloc_bytes - procfs_lifo_meminfo.len);
		memcpy(buff + copied, node->data, used);
		copied += used;
	    } else {
		memcpy(buff + copied, node->data, PROCFS_LIFO_CHUNK_SIZ);
		copied += PROCFS_LIFO_CHUNK_SIZ;
	    }
	}
	i++;
    }
    
    if (copy_to_user(buf, buff, len)) {
	pr_warn("procfs_lifo read() did not copy_to_user all bytes\n");
    }

    return copied;
}

static ssize_t procfs_lifo_write(struct file *file, const char *buf, size_t len, loff_t *off) {

    if (mutex_lock_interruptible(&procfs_lifo_mutex)) {
	pr_err("procfs_lifo write() couldn't lock mutex\n");
	return -1;
    }

    struct node *node;
    struct node *new_node = NULL;
    uint64_t to_copy = 0, total_copied = 0, unused = 0;
    int ret = 0;

    if (!(list_empty(&head_node))) {
        unused = procfs_lifo_meminfo.total_alloc_bytes - procfs_lifo_meminfo.len;
        to_copy = min(unused, len);
        list_for_each_entry(node, &head_node, list) {
	    if (list_is_last(&node->list, &head_node)) {
	        if (copy_from_user(node->data, buf, to_copy)) {
		    pr_err("procfs_lifo couldn't copy_from_user\n");
		    goto err;
		}

	        procfs_lifo_meminfo.len += to_copy;
		total_copied += to_copy;
	    }
        }
    }

    while (total_copied != len) {
	to_copy = min(PROCFS_LIFO_CHUNK_SIZ, len - total_copied);
	new_node = (struct node *)kzalloc(sizeof(struct node), GFP_KERNEL);
	if (!new_node) {
	    pr_err("procfs_lifo couldn't allocate new linked list node\n");
	    goto err;
	}

	procfs_lifo_meminfo.total_alloc_bytes += PROCFS_LIFO_CHUNK_SIZ;
	INIT_LIST_HEAD(&new_node->list);
	list_add_tail(&new_node->list, &head_node);
	procfs_lifo_meminfo.nodes++;
	if (copy_from_user(new_node->data, buf + total_copied, to_copy)) {
	    pr_err("procfs_lifo couldn't copy from user\n");
	    goto err;
	}

	procfs_lifo_meminfo.len += to_copy;
	total_copied += to_copy;
    }

    mutex_unlock(&procfs_lifo_mutex);
    return total_copied;

err:
    mutex_unlock(&procfs_lifo_mutex);
    return -1;
}

static long procfs_lifo_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
	case GET_STATS:
	    if (copy_to_user((struct procfs_lifo_meminfo_t *)arg, &procfs_lifo_meminfo, sizeof(procfs_lifo_meminfo))) {
	        pr_err("procfs_lifo_ioctl failure\n");
	    }
	    break;
	case 
	default:
	    break;
    }

    return 0;
}

static int __init procfs_lifo_init(void) {
    parent = proc_mkdir("sm_canonical", NULL);
    if (!parent) {
	    pr_err("Error creating proc parent directory\n");
    	    return -1;
    }

    if (!proc_create("procfs_lifo", 0666, parent, &fops)) {
	pr_err("Error creating proc file\n");
	goto r_parent;
    }

    pr_info("procfs_lifo loaded successfully\n");

    return 0;

r_parent:
    proc_remove(parent);
    return -1;
}

static void __exit procfs_lifo_exit(void) {
    struct node *node, *temp;
    list_for_each_entry_safe(node, temp, &head_node, list) {
        list_del(&node->list);
        kfree(node);
    }

    proc_remove(parent);
    pr_info("procfs_lifo removed successfully\n");
}

module_init(procfs_lifo_init);
module_exit(procfs_lifo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sujit Malde <sujitmalde@yahoo.co.uk>");
MODULE_DESCRIPTION("A last-in first-out buffer implemented for the proc filesystem.");
MODULE_VERSION("1.0");
