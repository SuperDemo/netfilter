/**
 * dealConf主要用来提供配置读取、数据包合法检查与提取、ip格式转换等功能
 * @return
 */

# define CONTENTMAXLEN 100

// 全局变量声明
extern char direction[CONTENTMAXLEN];
extern char titlecontent[CONTENTMAXLEN];
extern char content_flag[CONTENTMAXLEN];
extern char isapi[CONTENTMAXLEN];
extern char content[CONTENTMAXLEN];
extern char action[CONTENTMAXLEN];
extern char sourceip[CONTENTMAXLEN];
extern char targetip[CONTENTMAXLEN];

// 读取配置文件
char* readConf(void);

// 解析xml数据
int parseConf(char* data);

// 判断字符串是否符合要求格式
int isLegal(char* data);

// 提取字符串data中至少后由标签<content></content>包含的长度至少为minlen最大为maxlen的内容，保存在dest中
void extract(char* dest, char* content, char* data, int minlen, int maxlen);

// 将保存在整数in中的16进制格式ip地址转成点分10进制的字符串保存在sip中并返回该地址
char *in_ntoa(char *sip, __u32 in);

// 将无符号单个字符转换成字符串表示的数字
char *char2string(char *str, unsigned char c);