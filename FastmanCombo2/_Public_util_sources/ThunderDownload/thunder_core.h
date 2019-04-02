#ifndef __THUNDER_CORE_DENGXIAN_2014_12_30_HH__
#define __THUNDER_CORE_DENGXIAN_2014_12_30_HH__
#include "download_engine.h"
// 
// #define NULL ((void*)0)

class down_file_object
{
public:
	down_file_object();
	~down_file_object();

	// ���ؽӿ�
	int add_task(char* szurl, char* szpath = 0, char* szname = 0);
	int continue_task(char* szurl = 0, char* szpath = 0, char* szname = 0);
	int pause_task();
	int del_task(bool del_sucess_file); // �����Ƿ���������ɵ��ļ�
	int start();

	// ��ѯ�������
	__int64 get_current_size();  // Ŀǰ���ص����ݴ�С
	__int64 get_file_size();  // �ļ��ܴ�С
	int get_down_status();

	// ��ѯһЩ��Ϣ
	double query_down_pos(); // ��ѯ���ؽ���
	const char* get_down_file_name();  // ���ر�������
	const char* get_down_url();  // ��ѯ����url
	const char* get_save_path();  // �õ�����·��

private:
	char* m_down_url;
	char* m_save_path;
	char* m_save_name;
	__int64 m_file_size;
	TASKHANDLE m_task_handle;
	bool m_is_pause;
};





























#endif // __THUNDER_CORE_DENGXIAN_2014_12_30_HH__

