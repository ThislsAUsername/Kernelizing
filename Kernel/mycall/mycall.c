#include <linux/syscalls.h>

asmlinkage long sys_my_syscall(void)

{

	printk(KERN_INFO "This is the new system call that Josiah White implemented.\n");
	return 0;

}
