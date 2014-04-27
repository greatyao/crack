/* CrackManager.h
 *
 * Coordnator among several crack algorithms
 * Copyright (C) 2014 TRIMPS
 *
 * Craeted By YAO Wei at  03/21/2014
 */
 
#ifndef _CRACK_MANAGER_H_
#define _CRACK_MANAGER_H_

class Crack;
struct crack_block;

extern int exit_signal;

class CrackManager{
public:
	static CrackManager& Get();
	int Init();
	
	void Destroy();
	
	void RegisterCallback(int (*done)(char*, bool, const char*, bool), 
						  int (*status)(char*, int, float, unsigned int));
	
	//���ܽӿ�
	bool CouldCrack()const;
	int StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short* deviceIds, int ndevices,
					char* toolname = 0, int size = 0);
	int StopCrack(const char* guid);
	
	//����workitem�Ĳ���
	int CheckParameters(crack_block* item);
	
	//��ȡ�����ļ���·��
	void GetFilename(const char* guid, char* filename, int size)const;
	
	//��ȡ�ֵ�·��
	bool GetDict(unsigned char dict, char* dict_name, int size, char* rule_name = 0, int size2 = 0)const;
	
	//�Ƿ����CPU����
	bool UsingCPU()const;
	
private:
	CrackManager();
	~CrackManager();
	
	Crack** tools;
	int toolCount;
	int toolPriority;
};

#endif