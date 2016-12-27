#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>

#include "log.h"
#include "dealConf.h"
#include "netLink.h"

// 供解析xml字符串使用的临时缓冲区
static char newcontent[CONTENTMAXLEN];

// 对解析结果的变量进行定义
char direction[CONTENTMAXLEN];
char titlecontent[CONTENTMAXLEN];
char content_flag[CONTENTMAXLEN];
char isapi[CONTENTMAXLEN];
char content[CONTENTMAXLEN];
char action[CONTENTMAXLEN];
char sourceip[CONTENTMAXLEN];
char targetip[CONTENTMAXLEN];

#define MINAL_SIZE 97   //数据包的最小长度

// 规范数据包示例
// <?xml version='1.0' encoding='gb2312' ?><note id='13'><title>hello</title><message>world</message></note>

char head[50] = "<?xml version='1.0' encoding='gb2312' ?>";     // 长度为40
char data[1000];

char *readConf(void) {
    //从配置文件中读取配置

    struct file *fp;
    mm_segment_t fs;
    loff_t pos;

    // 打开配置文件
    fp = filp_open("../conf/netfilter.conf", O_RDONLY | O_CREAT, 0644);
    if (IS_ERR(fp)) {
        WARNING("open netfilter.conf failed!");
        return NULL;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);  // 确保内核能进行系统调用，使访问文件
    pos = 0;
    vfs_read(fp, data, sizeof(data), &pos); // 读取配置文件

    filp_close(fp, NULL);
    set_fs(fs);

    return data;
}

void extract(char* dest, char* content, char* data, int minlen, int maxlen){
    // 从数据包data中提取content形式的数据保存在dest中，要求数据长度不小于minlen

    char *start, *end;

//    new_content = (char *) kmalloc(strlen(content) + strlen("</>") + 1, GFP_KERNEL);  // 在内核中分配内存
//
//    if (!new_content){
//        ERROR("kmalloc new_content failed!\n");
//        dest[0] = '\0';     // 分配内存失败则将返回字符串置空
//        return;
//    }

    // 对content长度检查，防止缓冲区溢出
    if (strlen(content) + 3 > CONTENTMAXLEN){
        WARNING("content \"%s\" is too long", content);
        dest[0] = '\0';
        return;
    }
    sprintf(newcontent, "<%s>", content);

//    strcpy(new_content, " <");
//    strcat(new_content, content);
//    strcat(new_content, ">");

    // 先匹配<content>这样的标签头
    start = strstr(data, newcontent);
    if (!start){
        // 匹配失败则置空字符串返回
        WARNING("cannot find %s", newcontent);
        dest[0] = '\0';
//        kfree(new_content);
        return;
    }
    // 匹配成功则从后面继续往后匹配
    start += strlen(newcontent);
    data = start;

    sprintf(newcontent, "</%s>", content);
//    new_content[0] = '<';
//    new_content[1] = '/';

    // 然后匹配</content>这样的标签尾
    end = strstr(data + minlen, newcontent);
    if (!end){
        // 匹配失败则置空字符串返回
        WARNING("cannot find %s\n", newcontent);
        dest[0] = '\0';
//        kfree(new_content);
        return;
    }

    if (end - start <= 0){
        // 内容为空的意外情况
        INFO("<%s></%s> is empty!\n", content, content);
        dest[0] = '\0';
//        kfree(new_content);
        return;
    }
    // 一切正常则将标签内内容复制到dest中,如果数据过多，采取按maxlen截断策略
    strncpy(dest, start, end - start >= maxlen ? (maxlen - 1) : (end - start));
    dest[end - start] = '\0';

//    kfree(new_content); //释放分配的内存
}

int parseConf(char* data){
    // 解析xml数据

    extract(direction, "direction", data, 0, CONTENTMAXLEN);
    extract(content_flag, "content_flag", data, 0, CONTENTMAXLEN);
    extract(content, "content", data, 0, CONTENTMAXLEN);
    extract(sourceip, "sourceip", data, 0, CONTENTMAXLEN);
    extract(targetip, "targetip", data, 0, CONTENTMAXLEN);
    extract(action, "action", data, 0, CONTENTMAXLEN);
    extract(titlecontent, "titlecontent", data, 0, CONTENTMAXLEN);
    extract(isapi, "isapi", data, 0, CONTENTMAXLEN);

    DEBUG("titlecontent:%s, "
                  "direction:%s, "
                  "sourceip:%s, "
                  "targetip:%s, "
                  "action:%s, "
                  "content_flag:%s, "
                  "content:%s, "
                  "isapi:%s\n",
          titlecontent, direction, sourceip, targetip, action, content_flag, content, isapi);

    return 0;
}

int isLegal(char* data){
    // 检查数据包data合法性

    if (!data || strlen(data) < MINAL_SIZE){
        return 0;
    }

    if (strncmp(data, head, strlen(head)) != 0) {    // 不包含协议头不合法
        return 0;
    }
    data += strlen(head);

    if ((data = strstr(data, "<note id=")) == NULL) {    // 不包含<note结构不合法
        return 0;
    }
    data += strlen("<note id=\"0\">");

    if ((data = strstr(data, "<title>")) == NULL) {  // 不包含title不合法
        return 0;
    }
    data += strlen("<title>");

    if ((data = strstr(data, "</title>")) == NULL){
        return 0;
    }
    data += strlen("</title>");

    if ((data = strstr(data, "<message>")) == NULL) {    // 不包含message不合法
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

char *in_ntoa(char *sip, __u32 in) {
    // 此函数用来将16进制的ip地址in转换成点分十进制格式保存在sip中

    unsigned char *p = (char *) &in;
    char str[10];
    int i;

    strcpy(sip, "\"");  // 以双引号开始
    for (i = 0; i < 3; i++) {
        // 循环将单个字符转成数字并以点号分隔
        strcat(sip, char2string(str, *(p + i)));
        strcat(sip, ".");
    }
    // 转换最后一个字符，并加双引号结尾
    strcat(sip, char2string(str, *(p + 3)));
    strcat(sip, "\"");

    return sip;
}

char *char2string(char *str, unsigned char c) {
    // 此函数将无符号单个字符转换成字符串表示的数字

    int num;
    num = (int) c;  // 先将无符号字符转成整数

    // 根据整数位数不同按位取出放置在数组中
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