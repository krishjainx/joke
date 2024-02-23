#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "joke"
#define CLASS_NAME "joker"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Krish Jain");
MODULE_DESCRIPTION(":)");
MODULE_VERSION("0.2");

static int majorNumber;
static char message[256] = {0};
static short size_of_message;
static int number_of_opens = 0;
static struct class *jokeClass = NULL;
static struct device *jokeDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init joke_init(void)
{
    printk(KERN_INFO "Joke: Initializing the Joke LKM\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ALERT "Joke failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Joke: Registered correctly with major number %d\n", majorNumber);

    // Correctly calling class_create with only the CLASS_NAME as its argument
    jokeClass = class_create(CLASS_NAME);
    if (IS_ERR(jokeClass))
    {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(jokeClass);
    }
    printk(KERN_INFO "Joke: Device class registered correctly\n");

    jokeDevice = device_create(jokeClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(jokeDevice))
    {
        class_destroy(jokeClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(jokeDevice);
    }
    printk(KERN_INFO "Joke: Device class created correctly\n");
    return 0;
}

static void __exit joke_exit(void)
{
    device_destroy(jokeClass, MKDEV(majorNumber, 0));
    class_unregister(jokeClass);
    class_destroy(jokeClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "Joke: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    number_of_opens++;
    printk(KERN_INFO "Joke: Device has been opened %d time(s)\n", number_of_opens);
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int error_count = 0;
    if (*offset > 0)
    {
        // If position is beyond 0, return 0 indicating end of file
        return 0;
    }

    error_count = copy_to_user(buffer, message, size_of_message);
    if (error_count == 0)
    {
        printk(KERN_INFO "Joke: Sent %d characters to the user\n", size_of_message);
        *offset = size_of_message; // Update the offset to indicate the end of file
        return size_of_message;    // Return the number of characters sent
    }
    else
    {
        printk(KERN_ALERT "Joke: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;
    }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    // Clear the message buffer to prevent residual data from previous writes
    memset(message, 0, sizeof(message));

    // Limit the amount of data copied to the size of the message buffer minus 1
    // to leave space for the null terminator
    size_t bytes_to_copy = min(len, sizeof(message) - 1);

    // Copy the data from user space to kernel space safely
    if (copy_from_user(message, buffer, bytes_to_copy))
    {
        return -EFAULT;
    }

    // Ensure the message is null-terminated
    message[bytes_to_copy] = '\0';

    // Log the message, consider adding additional checks or logic as needed
    printk(KERN_INFO "Received message: %s\n", message);

    // Return the number of bytes copied to signify success
    return bytes_to_copy;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "Joke: Device successfully closed\n");
    return 0;
}

module_init(joke_init);
module_exit(joke_exit);