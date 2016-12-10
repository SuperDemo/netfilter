#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include<linux/kernel.h>
#include<linux/string.h>
#include <linux/slab.h>

#include "log.h"
#include "dealConf.h"

#define MINAL_SIZE 97
//example
//char data[150] = "<?xml version='1.0' encoding='gb2312' ?><note id='13'><title>hello</title><message>world</message></note>";

char head[50] = "<?xml version='1.0' encoding='gb2312' ?>";     //strlen=40
char data[1000];

char *readConf(void) {

    struct file *fp;
    mm_segment_t fs;
    loff_t pos;

    fp = filp_open("../conf/netfilter.conf", O_RDONLY | O_CREAT, 0644);
    if (IS_ERR(fp)) {
        WARNING("打开配置文件失败！");
        return NULL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    pos = 0;
    vfs_read(fp, data, sizeof(data), &pos);

    filp_close(fp, NULL);
    set_fs(fs);

    return data;
}

int isLegal(char* data){

    if (!data || strlen(data) < MINAL_SIZE){
        return 0;
    }

    if (strncmp(data, head, strlen(head)) != 0){
        return 0;
    }
    data += strlen(head);

    if ((data = strstr(data, "<note id=")) == NULL){
        return 0;
    }
    data += strlen("<note id=\"0\">");

    if ((data = strstr(data, "<title>")) == NULL){
        return 0;
    }
    data += strlen("<title>0");

    if ((data = strstr(data, "</title>")) == NULL){
        return 0;
    }
    data += strlen("</title>");

    if ((data = strstr(data, "<message>")) == NULL){
        return 0;
    }
    data += strlen("<message>");

    if ((data = strstr(data, "</message>")) == NULL){
        return 0;
    }
    data += strlen("</message>");

    if ((data = strstr(data, "</note>")) == NULL){
        return 0;
    }

    return 1;
}

void extract(char* dest, char* content, char* data, int minlen){
    // dest is ans, content like "message", data is source, minlen is min length of context in content label
    char *new_content, *start, *end;

    new_content = (char*)kmalloc(strlen(content) + strlen("</>") + 1, GFP_KERNEL);

    if (!new_content){
        printk(KERN_EMERG "kmalloc new_content failed!\n");
    }

    strcpy(new_content, " <");
    strcat(new_content, content);
    strcat(new_content, ">");
    // search <content>
    start = strstr(data, new_content + 1);
    if (!start){
        printk(KERN_EMERG "cannot find %s\n", new_content + 1);
        dest[0] = '\0';
        kfree(new_content);
        return;
    }
    start += strlen(new_content + 1);
    data = start;

    new_content[0] = '<';
    new_content[1] = '/';

    // search </content>
    end = strstr(data + minlen, new_content);
    if (!end){
        printk(KERN_EMERG "cannot find %s\n", new_content);
        dest[0] = '\0';
        kfree(new_content);
        return;
    }

    if (end - start <= 0){
        printk(KERN_EMERG "<%s></%s> is empty!\n", content, content);
        dest[0] = '\0';
        kfree(new_content);
        return;
    }
    strncpy(dest, start, end - start);
    dest[end - start] = '\0';

    kfree(new_content);
}

char *in_ntoa(char *sip, __u32 in) {

    unsigned char *p = (char *) &in;
    char str[10];
    int i;

    strcpy(sip, "\"");
    for (i = 0; i < 3; i++) {
        strcat(sip, char2string(str, *(p + i)));
        strcat(sip, ".");
    }
    strcat(sip, char2string(str, *(p + 3)));
    strcat(sip, "\"");

    return sip;
}

char *char2string(char *str, unsigned char c) {
    int num;
    num = (int) c;

    if (num >= 100) {
        str[0] = num / 100 + '0';
        num %= 100;
        str[1] = num / 10 + '0';
        num %= 10;
        str[2] = num + '0';
        str[3] = '\0';
    } else if (num >= 10) {
        str[0] = num / 10 + '0';
        num %= 10;
        str[1] = num + '0';
        str[2] = '\0';
    } else {
        str[0] = num + '0';
        str[1] = '\0';
    }
    return str;
}