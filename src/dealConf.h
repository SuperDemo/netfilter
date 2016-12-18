/**
 * dealConf主要用来提供配置读取、数据包合法检查与提取、ip格式转换等功能
 * @return
 */


char* readConf(void);

int isLegal(char* data);
void extract(char* dest, char* content, char* data, int minlen);

char *in_ntoa(char *sip, __u32 in);

char *char2string(char *str, unsigned char c);