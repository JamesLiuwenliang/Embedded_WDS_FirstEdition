
#include "stdio.h"

/*������x86ƽ̨��,GCC������Ĭ�ϰ�4���ֽڶ���*/
struct person
{
	char *name;
	int age;
	char score;
	int id;
};
/*�ɱ����֮���Բ����������䷽��,��Ҫԭ�������ݴ��ɿɱ����ʱ�����������ռ���*/
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

	struct person per = {"www.100ask.org",10,'A',123};/*sizeof(person)��16,��Ϊx86��4λ����*/


	push_test("abcdefdg\r\n",123,per);


	return 0;
}




