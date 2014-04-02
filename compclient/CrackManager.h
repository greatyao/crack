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

class CrackManager{
public:
	static CrackManager& Get();
	int Init();
	
	void RegisterCallback(int (*done)(char*, bool, const char*), 
						  int (*status)(char*, int, float, unsigned int));
	
	//���ܽӿ�
	int StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short deviceId);
	int StartCrack(const crack_block* item, const char* guid, bool gpu, unsigned short* deviceIds, int ndevices);
	int StopCrack(const char* guid);
	
	//��ȡ�����ļ���·��
	void GetFilename(const char* guid, char* filename, int size)const;
	
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