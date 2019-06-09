#include<stdlib.h>
#include<stdio.h>
#include<iostream>

using namespace std;

//Base 64编码表
const char base_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int myEncode(const uint8_t* bindata, char* base64, int binlength){
	int i, j;
	uint8_t current;
	/*每次处理三个字符*/
	for (i = 0, j = 0; i < binlength; i += 3) {
		/*前6个bit 首先右移两位*/
		current = (bindata[i] >> 2);
		current &= (uint8_t)0x3F;//0x3F == 00111111 指定取后6位字节的数
		base64[j++] = base_table[(int)current];//switchTbale
		
		/*第一个字节的最后两个二进制位*/
		current = ((uint8_t)(bindata[i] << 4)) & ((uint8_t)0x30);//0x30 == 00110000 指定取第3、4位字节的数
		if (i + 1 >= binlength) {//不足位的时候，自动补足
			base64[j++] = base_table[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}

		/*9~12bit, 并连接7~8bit*/
		current |= ((uint8_t)(bindata[i + 1] >> 4)) & ((uint8_t)0x0F);
		base64[j++] = base_table[(int)current];//switchTbale
		
		/*13~16bit*/
		current = ((uint8_t)(bindata[i + 1] << 2)) & ((uint8_t)0x3C);//0x3c == 00111100 取指定的第2、3、4、5位
		
		/*就此结尾*/
		if (i + 2 >= binlength) {
			base64[j++] = base_table[(int)current];
			base64[j++] = '=';
			break;
		}
		
		/*17~18bit, 并连接13~16bit*/
		current |= ((uint8_t)(bindata[i + 2] >> 6)) & ((uint8_t)0x03);//0x3 == 00000011 取指定的第7、8位  后4位和前2位拼接
		base64[j++] = base_table[(int)current];//switchTbale
		
		/*19~24bit*/
		current = ((uint8_t)bindata[i + 2]) & ((uint8_t)0x3F);//0x3F == 00111111 取指定的第3、4、5、6、7、8
		base64[j++] = base_table[(int)current];//switchTbale
	}
	base64[j] = '\0';
	return j;
}

static int base64_decode(const uint8_t* bindata, char* base64, int binlength)
{
	int i, j;
	uint8_t current;
	
	//4个字母为一组一起处理
	int mybindata[4];
	for (i = 0, j = 0; i < binlength; i += 4) {
		for (int h = 0; h < 64; h++)
		{
			if (bindata[i] == base_table[h])
			{
				mybindata[0] = h;
				break;
			}
		}
		for (int h = 0; h < 64; h++)
		{
			if (bindata[i + 1] == base_table[h])
			{
				mybindata[1] = h;
				break;
			}
		}
		for (int h = 0; h < 64; h++)
		{
			if (bindata[i + 2] == base_table[h])
			{
				mybindata[2] = h;
				break;
			}
		}
		for (int h = 0; h < 64; h++)
		{
			if (bindata[i + 3] == base_table[h])
			{
				mybindata[3] = h;
				break;
			}
		}

		current = (mybindata[0] << 2);
		current |= ((uint8_t)(mybindata[1] >> 4)) & ((uint8_t)0x03);
		base64[j++] = (char)current;

		current = (mybindata[1] << 4);
		current |= ((uint8_t)(mybindata[2] >> 2)) & ((uint8_t)0x0F);
		base64[j++] = (char)current;

		current = (mybindata[2] << 6);
		current |= ((uint8_t)(mybindata[3]));
		base64[j++] = (char)current;
	}

	if (bindata[binlength - 2] == '='){
		base64[j - 1] = ' ';
		base64[j - 2] = ' ';
		base64[j - 2] = '\0';
		return j;

	}
	if (bindata[binlength - 1] == '='){
		base64[j - 1] = ' ';
		base64[j - 1] = '\0';
		return j;
	}
	base64[j] = '\0';
	return j;
}
void main(void) {
	char text[] = "a";
	char* str = text;
	printf("input: %s \n", str);
	char* base64_str = (char* )calloc(1, 1024);
	myEncode((const uint8_t*)str, base64_str, strlen(str));
	printf("encode base64: %s \n", base64_str);

	char* debase64_str = (char*)calloc(1, 1024);
	base64_decode((const uint8_t*)base64_str, debase64_str, strlen(base64_str));
	printf("decode base64: %s \n", debase64_str);

	free(debase64_str);
	free(base64_str);
}

