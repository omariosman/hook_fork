#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/moduleparam.h>
#include<linux/fs.h>
#include<linux/buffer_head.h>
#include<asm/segment.h>
#include<asm/uaccess.h>
#include<linux/slab.h>
#include<linux/kallsyms.h>
#include<linux/stat.h>


MODULE_LICENSE("GPL");

//char *sym_name = "sys_call_table";
//typedef asmlinkage long(*sys_call_ptr_t)(const struct pt_regs*);
//static sys_call_ptr_t *sys_call_table;
typedef asmlinkage long(*fork_prototype)(unsigned long flags, void *stack, int *parent_tid, int *child_tid, unsigned long tls);
static sys_call_ptr_t* sys_call;

//Declaration of my new function from the type customized fork
fork_prototype original_fork;



int fork_counter = 0;
static asmlinkage long new_fork(unsigned long flags, void *stack, int *parent_tid, int *child_tid, unsigned long tls){
	fork_counter++;
	if (fork_counter % 10 == 0){
		printk("Fork Invoked %d times\n", fork_counter);
	}
	//calling the original fork
	return original_fork(flags, stack, parent_tid, child_tid, tls);
}





struct myfile{
	struct file*f;
	mm_segment_t fs;
	loff_t pos;
};

struct myfile *open_file_for_read(char *filename){
	struct myfile *myfile_obj;
	myfile_obj = kmalloc(sizeof(struct myfile), GFP_KERNEL);

	
	
	int err = 0;

	myfile_obj->fs = get_fs();
	set_fs(get_ds());
	myfile_obj->f = filp_open(filename, O_RDONLY,0);
	myfile_obj->pos = 0;


	set_fs(myfile_obj->fs);

	if(IS_ERR(myfile_obj->f)){
		err = PTR_ERR(myfile_obj->f);
		return NULL;
	}

	return myfile_obj;

}

void close_file(struct myfile *mf){
	filp_close(mf->f, NULL);
	kfree(mf);
}

int read_from_file_until(struct myfile *mf, char *buf, unsigned long vlen, char c){
	
	int ret = 0;
	mf->fs = get_fs();
	set_fs(get_ds());
	
	int i;
	for (i = 0; i < vlen; i++){
		ret = vfs_read(mf->f, buf + i,1, &(mf->pos));
		if ((buf[i] == c) || (buf[i]) == '\0'){
			break;
		}
	}

	i++;
	buf[i] = '\0';
	set_fs(mf->fs);
	return ret;
	
}

struct file *fp;
static int hello_init(void){

	//set write bit
	//write_cr0(read_cr0() & (~0x10000));
	struct myfile* mf = open_file_for_read("/proc/version");
	char *data;
	data = kmalloc(sizeof(char), GFP_KERNEL);
	read_from_file_until(mf, data, 1024, ' ');
	read_from_file_until(mf, data, 1024, ' ');
	read_from_file_until(mf, data, 1024, ' ');

	printk("Kernel Version: %s\n", data);

	//char system_map[100] = "/boot/System.map-";
	//strncat(system_map, data, 20);
	//printk("Name of the file: %s\n", system_map);
	struct myfile* sysmap = open_file_for_read("/boot/System.map-4.19.0-13-amd64");
	char *line;
	line = kmalloc(100*sizeof(char), GFP_KERNEL);

	char *word;
	word = kmalloc(100*sizeof(char), GFP_KERNEL);
	word = "sys_call_table";

	while(true){
		read_from_file_until(sysmap, line, 1024, '\n');
		if (strstr(line, word)){
			break;
		}
	}

	printk("syscall: %s\n", line);

	char *sys_call_addr;
	sys_call_addr = kmalloc(100*sizeof(char), GFP_KERNEL);
	strncpy(sys_call_addr, line, 16);
	sys_call_addr[16] = '\0';
	printk("sys call addr: %s\n", sys_call_addr);

	sys_call_ptr_t *sys_call_arr = (sys_call_ptr_t *) sys_call_addr;
	printk("Fork Address: %px\n", sys_call_arr[__NR_fork]);

	unsigned long laddr;
	sscanf(sys_call_addr, "%lx", &laddr);
	sys_call = (sys_call_ptr_t*)laddr;


	//Caching the old fork into variable called original_fork
	original_fork = (fork_prototype)sys_call[__NR_clone];
	unsigned long mycr = read_cr0();
	clear_bit(16, &mycr);
	asm volatile("mov %0, %%cr0": "+r" (mycr), "+m" (__force_order));
	sys_call[__NR_clone] = (sys_call_ptr_t)new_fork;
	mycr = read_cr0();
	set_bit(16, &mycr);
	asm volatile ("mov %0, %%cr0": "+r" (mycr), "+m" (__force_order));

	close_file(mf);
	kfree(data);
	return 0;
}





static void hello_exit(void){
	unsigned long mycr = read_cr0();
	clear_bit(16, &mycr);
	asm volatile("mov %0, %%cr0": "+r" (mycr), "+m" (__force_order));
	//Restoring the old fork in its place again
	sys_call[__NR_clone] = (sys_call_ptr_t)original_fork;
	//deassert write bit
	mycr = read_cr0();
	set_bit(16, &mycr);
	asm volatile("mov %0, %%cr0": "+r" (mycr), "+m" (__force_order));
	printk(KERN_ALERT "Bye Bye CSCE-3402 :)\n");
	
}

module_init(hello_init);
module_exit(hello_exit);
