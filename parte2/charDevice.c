#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>


// Prototypes
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
//static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_read(struct file *, char * __user, size_t, loff_t *);
//static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 80

// Global variables
static int Major;
static int Device_Open = 0;
//static char msg[BUF_LEN];
//static char *msg_Ptr;
//static char message[256] = {0};
//static short size_of_message;

static char Message[BUF_LEN];
static char *Message_Ptr;

static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

// Function is called when the module is loeaded
int init_module(void)
{
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) {
		printk(KERN_ALERT "Registering char device failed with %d\n", Major);
		return Major;
	}
	printk(KERN_INFO "I was assigned major number %d.\n", Major);
	printk(KERN_INFO "Create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	return SUCCESS;
}

//Unregister the device
void cleanup_module(void)
{
	unregister_chrdev(Major, DEVICE_NAME);
	//int ret = unregister_chrdev(Major, DEVICE_NAME);
	//if (ret < 0)
		//printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);
	
	printk(KERN_INFO "Unregistering char device success\n");
}


/*
 * Methods
 */

/*
 * Called when a process writes to dev file: echo "hi" > /dev/hello
 */
static ssize_t device_write(struct file *file,
	     const char __user * buffer, size_t length, loff_t * offset)
{
	int i;

	printk(KERN_INFO "device_write(%s)\n", buffer);
	printk(KERN_INFO "device_write(%zu)", length);

	for (i = 0; i < length && i < BUF_LEN; i++)
		get_user(Message[i], buffer + i);

	Message_Ptr = Message;

	return i;
}


/*
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *file,
			   char __user * buffer,
			   size_t length,
			   loff_t * offset)
{	
	int bytes_read = 0;
	printk(KERN_INFO "comienzo\n");

	printk(KERN_INFO "device_read(%p,%p,%zu)\n", file, buffer, length);

	if (*Message_Ptr == 0)
		return 0;

	while (length && *Message_Ptr) {
		put_user(*(Message_Ptr++), buffer++);
		length--;
		bytes_read++;
	}

	printk(KERN_INFO "Read %d bytes, %zu left\n", bytes_read, length);

	return bytes_read;
}



/*
 * Called when a process tries to open the device file, like
 */
static int device_open(struct inode *inode, struct file *file)
{
        if (Device_Open)
                return -EBUSY;

        Device_Open++;

        printk(KERN_INFO "Device has been opened %d times\n", Device_Open);

	Message_Ptr = Message;

        try_module_get(THIS_MODULE);
        return SUCCESS;
}

/*
 * Called when a process closes the device file.
 * Basicamente reduce 1 la variable Device_Open
 */
static int device_release(struct inode *inode, struct file *file)
{
        Device_Open--;

        printk(KERN_INFO "Device successfully close\n");

        module_put(THIS_MODULE);

        return 0;
}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("UNGS");
MODULE_DESCRIPTION("Primer char device driver");
