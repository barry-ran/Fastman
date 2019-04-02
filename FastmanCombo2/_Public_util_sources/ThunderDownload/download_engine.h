#ifndef __DOWNLOAD_ENFINE_DENGXIAN_2014_12_30_HH__
#define __DOWNLOAD_ENFINE_DENGXIAN_2014_12_30_HH__

#pragma comment(lib, "download_engine")

//////////////////////////////////////////////////////////////////////////
//  ���ݽṹ����
//////////////////////////////////////////////////////////////////////////
typedef void* TASKHANDLE;


#pragma pack(push, 1)
typedef struct _task_info
{
	int task_state; // 0 ��ʾ�������أ���������, == 2 ???, ==C  Ҫ���� get_failure_detail, == 
	unsigned long unknow_val[6];
	__int64 total_file_size;   // �ļ��ܴ�С
	__int64 flush_byte_size;   // ĿǰΪֹ�������ܵ���������
	__int64 down_file_size;    // �����������ļ���С
	unsigned long unknow_val2;
	unsigned long file_name_len;  // ���ر�����ļ�����
	char file_name[260];
	unsigned char unknow_data[1024];
}task_info, *ptask_info;
#pragma pack(pop)




//////////////////////////////////////////////////////////////////////////
// ��������
//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

// ������ʵû���壬�˰汾�е� set_product_identifier ֱ�ӷ���0
int __stdcall set_product_identifier(unsigned long unknow, char* marver, size_t mar_len, char* majver, size_t maj_len);
int __stdcall init(unsigned long mode); // ֱ�Ӵ�0������0
int __stdcall set_upload_speed_limit(int val1, int val2); // �����ϴ�����, -1 ������
int __stdcall cancel_speed_limit();  // ȡ������  -1
int __stdcall set_speed_limit(int val1);

// �¼���������
int __stdcall create_new_task(char* url, size_t url_len, 
					  char* buf1, size_t buf1_len, 
					  char* buf2, size_t buf2_len,
					  char* save_addr, size_t addr_len,
					  char* save_name, size_t name_len, // �������ִ��գ���ʾĬ������
					  TASKHANDLE* task);


// ������ǰ����������
int __stdcall create_continued_task(char* url, size_t url_len, 
	char* buf1, size_t buf1_len, 
	char* buf2, size_t buf2_len,
	char* save_addr, size_t addr_len,
	char* save_name, size_t name_len, // �������ִ��գ���ʾĬ������
	TASKHANDLE* task);

int __stdcall get_failure_detail(TASKHANDLE task, char* pbuf); 
int __stdcall set_task_type(TASKHANDLE task, unsigned long mode); 
int __stdcall start_task(TASKHANDLE task); 
int __stdcall stop_task(TASKHANDLE task);     // �ر�/��ͣ����
int __stdcall delete_task(TASKHANDLE task);   // ɾ������from�����б�

int __stdcall set_proxy_info(TASKHANDLE task, int, int);
int __stdcall query_task_info(TASKHANDLE task, ptask_info ptaskinfo);   // ��ѯ������Ϣ�������ļ��������ش�С��
int __stdcall get_task_gcid(TASKHANDLE,unsigned char*, int); 
int __stdcall query_part_cid(TASKHANDLE, ptask_info); 
 
int __stdcall uninit();      // �ر�ǰȡ����ʼ��


#ifdef __cplusplus
	};
#endif



#endif  // __DOWNLOAD_ENFINE_DENGXIAN_2014_12_30_HH__