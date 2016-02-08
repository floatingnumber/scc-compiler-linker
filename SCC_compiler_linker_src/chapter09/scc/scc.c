// ���Լ�����д��������������������Դ����

#include "scc.h"

FILE *fin = NULL;				// Դ�ļ�ָ��
char *filename;					// Դ�ļ�����
DynArray src_files;				// Դ�ļ�����
char outfile[256];				// ����ļ���
int output_type;				// ����ļ�����
float scc_version = 1.00;		// SCC�������汾��

/*********************************************************** 
 * ����:	������ڴ沢�����ݳ�ʼ��Ϊ'0'
 * size:	�����ڴ��С
 **********************************************************/
void *mallocz(int size)
{
    void *ptr;
	ptr = malloc(size);
	if (!ptr && size)
        error("�ڴ����ʧ��");
    memset(ptr, 0, size);
    return ptr;
}

/*********************************************************** 
 * ����:	�����ϣ��ַ
 * key:		��ϣ�ؼ���
 * MAXKEY:	��ϣ������
 **********************************************************/
int elf_hash(char *key)
{
    int h = 0, g;
    while (*key) 
	{
        h = (h << 4) + *key++;
        g = h & 0xf0000000;
        if (g)
            h ^= g >> 24;
        h &= ~g;
    }
    return h % MAXKEY;
}

/*********************************************************** 
 * ����:	�����ֽڶ���λ��
 * n:		δ����ǰֵ
 * align:   ��������
 **********************************************************/
int calc_align(int n, int align)
{                                                     
    return ((n + align - 1) & (~(align - 1)));        
}


/***********************************************************
 * ����:	��ʼ��
 **********************************************************/
void init ()
{
	dynarray_init(&src_files,1);
    init_lex();

	syntax_state = SNTX_NUL;
	syntax_level = 0;

   	stack_init(&local_sym_stack,8);
	stack_init(&global_sym_stack,8);
	sym_sec_rdata = sec_sym_put(".rdata",0);

	int_type.t = T_INT;
    char_pointer_type.t = T_CHAR;
    mk_pointer(&char_pointer_type);
	default_func_type.t = T_FUNC;
	default_func_type.ref = sym_push(SC_ANOM, &int_type, KW_CDECL, 0);

	optop = opstack - 1;
    
	init_coff();	

}

/*********************************************************** 
 * ����:	ɨβ��������
 **********************************************************/
void cleanup()
{	
	int i;
    sym_pop(&global_sym_stack, NULL);
	stack_destroy(&local_sym_stack);
	stack_destroy(&global_sym_stack);
	free_sections();
    
	for(i = TK_IDENT; i < tktable.count; i++)
	{	
        free(tktable.data[i]);	//tktable�����TK_IDENT�����ͷţ��������dynarray_free�������⣬��ΪTK_IDENT���µ�tokenû�з��ڶ��У����Ƿ��ھ�̬�洢��

	}
    free(tktable.data);
}


/***********************************************************
 * ����:	�õ��ļ���չ��
 * fname:	�ļ�����
 **********************************************************/
void get_obj_fname(char *fname)
{
	char *p;
	int i;
	p = strrchr(fname,'.');
	i = p - fname + 1;
    strcpy(outfile,fname);
	strcpy(outfile+i,"obj");
}

/*********************************************************** 
 * ����:	main������
 **********************************************************/
int main(int argc, char ** argv)
{ 
	char *fname;

	init();
    fname = argv[1];
	fin = fopen(fname,"rb");
	if(!fin)
		printf("cannot open SC source file");
	getch();
	line_num = 1;
	get_token();
	translation_unit();
	fclose(fin);

	get_obj_fname(fname);
	write_obj(outfile);

	printf("\n\n%s ��������: %d��\n\n",fname,line_num);
	cleanup();
	return 1;
}