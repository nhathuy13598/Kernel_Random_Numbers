# Kernel_Random_Numbers
Tạo ngẫu nhiên số trong Kernel Linux

## Cách sử dụng
Chọn Folder chứa mã nguồn, click phải chọn Open In Terminal
Gõ make -> sudo insmod Random.ko -> sudo ./test (Để chạy thử) -> dmesg (Để kiểm tra số phát sinh trong Kernel) -> sudo rmmod Random.ko (Remove Module)

## Giải thích mã nguồn
```
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    i = 0;
    get_random_bytes(&randomNumber, sizeof(char));
    printk(KERN_INFO "RANDOMMACHINE: Random number is %d\n", randomNumber);
    if (len < 4)
    {
        printk(KERN_INFO "\n\nRANDOMMACHINE: Failed\n");
        return -EFAULT;
    }
    if (randomNumber != 0)
    {
	
        while (randomNumber != 0)
        {
            temp[i] = randomNumber % 10 + '0';
            randomNumber = randomNumber / 10;
            i++;
        }
        temp[i] = '\0';
        buffer[i] = '\0';
        i -= 1;
        while (i >= 0)
        {
            *buffer = temp[i];
            i -= 1;
            buffer += 1;
        }
        return 0;
    }
    else
    {

        *(buffer++) = '0';
        *buffer = '\0';
        return 0;
    }
}
```
Ý tưởng: Random một số sau đó tách từng chữ số đưa vào chuỗi **buffer**.
Đầu vào là chuỗi **buffer**, độ dài **len** được gửi từ ứng dụng ở **User Space**.
Hàm ```get_random_bytes(&randomNumber, sizeof(char));``` dùng để random số có kích thước là **sizeof(char)** được lưu vào biến **randomNumber**.

 
