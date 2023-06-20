#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <deque>
#include <string>
#include <algorithm>

#define SIZE 640

struct block{
	int id; // id 为 0 表示空闲 
	int begin;
	int end;
	int size;
	int state; // 1 表示已分配 
	block *pre;
	block *next;
	
	block(int id, int size){
		this->id = id;
		begin = 0;
		end = size;
		this->size = size;
		state = 0;
		next = NULL;
	}
	
	block(int id, int size, int begin, int end){
		this->id = id;
		this->size = size;
		this->begin = begin;
		this->end = end;
		state = 0;
		next = NULL;
	}
};

int c; // 操作轮次 
block* table; // 内存块表 
deque<pair<int, int> > work; // 工作序列 

// 初始化内存块表以及工作表	
void init(){
	c = 0;
	table = new block(0,-1);
	table->state = -1;
	table->next = new block(0, SIZE);
	table->next->pre = table;
	work.push_back(make_pair(1,130));
	work.push_back(make_pair(2,60));
	work.push_back(make_pair(3,100));
	work.push_back(make_pair(2,-1));
	work.push_back(make_pair(4,200));
	work.push_back(make_pair(3,-1));
	work.push_back(make_pair(1,-1));
	work.push_back(make_pair(5,140));
	work.push_back(make_pair(6,60));
	work.push_back(make_pair(7,50));
	work.push_back(make_pair(6,-1));
}


// 删除内存块指针 
void delete_spare(block* s){
	delete(s);
	s = NULL;
	delete(s);
}

// 合并两个内存区间 
void merge(block* s1, block* s2){
	// 扩大 s1 结束位置 
	s1->end = s2->end;
	// 扩容 s1 
	s1->size += s2->size;
	// 新链接s1，将s2剔除 
	s1->next = s2->next;	
	if(s2->next){ s2->next->pre = s1; }
	// 删除节点 s2 
	delete_spare(s2);
}

// 释放编号为 id 的内存块 
void free(int id){
	block* p = table;
	while(p->id != id){
		p = p->next;
	}
	// 将 id 和状态置零，表示可分配 
	p->state = 0;
	p->id = 0;
	// 若前驱也为空闲区间，合并 
	if(p->pre->state == 0) { merge(p->pre, p); }
	// 若后继也为空闲区间，合并 
	if(p->next && p->next->state == 0) { merge(p, p->next); }
}

// 为当前任务分配内存块 
void alloc(pair<int, int> cur){
	block* p = table;	
	// 遍历得到可分配的内存块 
	while(p != NULL){
		if(p->state == 0 && p->size >= cur.second){
			break;
		}
		p = p->next;
	}
	if(!p){
		cout << "无剩余空间，分配失败" << endl;
		return; 
	}
	
	// 将状态置为已分配 
	p->id = cur.first;
	p->state = 1;
	int size = p->size - cur.second;
	// 若无剩余空间，直接退出 
	if(size == 0){
		return;
	}
	// 已分配空间的结束位置 
	int end = p->begin + cur.second;
	// 将剩余空间作为空闲分区，创建新节点 
	block* spare = new block(0, size, end, p->end);
	
	// 缩小当前已分配空间大小 
	p->size = cur.second;
	// 重新规定已分配空间结束位置 
	p->end = end;
	
	// 将新的空闲分区链上
	// 先把新节点链上链表 
	spare->next = p->next;
	spare->pre = p;
	// 撤销旧链，链上新节点 
	p->next = spare;
	if(spare->next) { spare->next->pre = spare; }
} 


// 从前往后打印内存块分配表 
void print_table(){
	cout << "第" << c << "轮\n======================================================================\n";
	cout << "内存编号\t起始地址\t终止地址\t内存大小\t已分配\n";
	cout << "----------------------------------------------------------------------\n";
	block* p = table->next;
	while(p){
		cout << p->id << "\t\t"
			 << p->begin << "\t\t" 
			 << p->end << "\t\t"
			 << p->size << "\t\t" 
			 << p->state << endl;
		cout << "----------------------------------------------------------------------\n";
		p = p->next;
	}
	cout << "\n\n" << endl;
}


// 最快适应算法 
void first_fit(){
	// 当工作序列不为空 
	while(!work.empty()){
		print_table();
		pair<int, int> cur = work.front();
		// 当为 -1，说明回收 
		if(cur.second == -1){
			free(cur.first);
		} else { // 分配新空间 
			alloc(cur);
		}
		work.pop_front();
		c++;
	}
	print_table();
}

bool sort_block(block* s1, block* s2){
	if(s1->state != s2->state){
		return s1->state < s2->state; // 1 为已占用，优先级很低 
	}	
	return s1->size > s2->size;
}

void sort(){
	vector<block*> vec;
	block* p = table->next;
	while(p){
		vec.push_back(p);
		p = p->next;
	}
	sort(vec.begin(), vec.end(), sort_block);
	p = table;
	for(int i = 0; i < vec.size(); i++){
		p->next = vec[i];
		vec[i]->pre = p;
		p = p->next;
	}
	p->next = NULL;
}

void best_fit(){
	// 当工作序列不为空 
	while(!work.empty()){
		sort();
		print_table();
		pair<int, int> cur = work.front();
		// 当为 -1，说明回收 
		if(cur.second == -1){
			free(cur.first);
		} else { // 分配新空间 
			alloc(cur);
		}
		work.pop_front();
		c++;
	}
	sort();
	print_table();
}

void run(int algorithm){
	switch(algorithm){
		case 1: first_fit(); break;
		case 2: best_fit(); break;
		default: break;
	}
}

int main(){

	init();
	cout << "请选择算法 (1 - 最快适应算法  or  2 - 最佳适应算法): ";
	int num;
	cin >> num;
	run(num); 
    return 0;
}
