
#include "stdio.h"

/*由于在x86平台下,GCC编译器默认按4个字节对齐*/
struct person
{
	char *name;
	int age;
	char score;
	int id;
};
/*可变参数之所以操作起来极其方便,主要原因是数据传成可变参数时都是在连续空间上*/
int push_test(const char *format, ...)
{
	char *p = (char *)&format;
	int i;
	struct person per;

	
	printf("arg1: %s \r\n",format);

	p = p+sizeof(char *);
	i = *((int *)p);
	printf("arg2: %d \r\n",i);


	p = p + sizeof(int);
	per = (struct person *)p;
	printf("arg3: name %s,age %d,score %c,id %d",per.name,per.age,per.score,per.id);
	
	return 0;
}

int main(int argc,char **argv)
{

	struct person per = {"www.100ask.org",10,'A',123};/*sizeof(person)是16,因为x86按4位对齐*/


	push_test("abcdefdg\r\n",123,per);


	return 0;
}




