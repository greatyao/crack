#ifndef __SPLIT_CHARS__H__
#define __SPLIT_CHARS__H__

#include <stdio.h>
#include <math.h>
#include <string>
#include "algorithm_types.h"

using namespace std;

//切割算法
class csplit
{
private:
	double compute_combinations(unsigned characters,unsigned len_max,unsigned len_min=1);
	
	string make_character_table(enum crack_charset);//生成字符集数组
	bool init_bf(unsigned len_min,unsigned len_max,char *character_set);//初始化
		
	struct crack_block *split_mask(struct crack_task *pct,unsigned &nsplits);
	struct crack_block *split_dic(struct crack_task *pct,unsigned &nsplits);
public:
	csplit();
	~csplit();
public:
	//nsplits保存切割以后的份数
	//返回指针数组,需要手工释放
	struct crack_block *split_easy(struct crack_task *pct,unsigned &nsplits);
	struct crack_block *split_intelligent(struct crack_task *pct,unsigned &nsplits);

	struct crack_block *split_task(struct crack_task *pct,unsigned &nsplits);

	void release_splits(char *p);//释放

};



#endif