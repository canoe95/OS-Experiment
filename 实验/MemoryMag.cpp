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
	int id; // id Ϊ 0 ��ʾ���� 
	int begin;
	int end;
	int size;
	int state; // 1 ��ʾ�ѷ��� 
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

int c; // �����ִ� 
block* table; // �ڴ��� 
deque<pair<int, int> > work; // �������� 

// ��ʼ���ڴ����Լ�������	
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


// ɾ���ڴ��ָ�� 
void delete_spare(block* s){
	delete(s);
	s = NULL;
	delete(s);
}

// �ϲ������ڴ����� 
void merge(block* s1, block* s2){
	// ���� s1 ����λ�� 
	s1->end = s2->end;
	// ���� s1 
	s1->size += s2->size;
	// ������s1����s2�޳� 
	s1->next = s2->next;	
	if(s2->next){ s2->next->pre = s1; }
	// ɾ���ڵ� s2 
	delete_spare(s2);
}

// �ͷű��Ϊ id ���ڴ�� 
void free(int id){
	block* p = table;
	while(p->id != id){
		p = p->next;
	}
	// �� id ��״̬���㣬��ʾ�ɷ��� 
	p->state = 0;
	p->id = 0;
	// ��ǰ��ҲΪ�������䣬�ϲ� 
	if(p->pre->state == 0) { merge(p->pre, p); }
	// �����ҲΪ�������䣬�ϲ� 
	if(p->next && p->next->state == 0) { merge(p, p->next); }
}

// Ϊ��ǰ��������ڴ�� 
void alloc(pair<int, int> cur){
	block* p = table;	
	// �����õ��ɷ�����ڴ�� 
	while(p != NULL){
		if(p->state == 0 && p->size >= cur.second){
			break;
		}
		p = p->next;
	}
	if(!p){
		cout << "��ʣ��ռ䣬����ʧ��" << endl;
		return; 
	}
	
	// ��״̬��Ϊ�ѷ��� 
	p->id = cur.first;
	p->state = 1;
	int size = p->size - cur.second;
	// ����ʣ��ռ䣬ֱ���˳� 
	if(size == 0){
		return;
	}
	// �ѷ���ռ�Ľ���λ�� 
	int end = p->begin + cur.second;
	// ��ʣ��ռ���Ϊ���з����������½ڵ� 
	block* spare = new block(0, size, end, p->end);
	
	// ��С��ǰ�ѷ���ռ��С 
	p->size = cur.second;
	// ���¹涨�ѷ���ռ����λ�� 
	p->end = end;
	
	// ���µĿ��з�������
	// �Ȱ��½ڵ��������� 
	spare->next = p->next;
	spare->pre = p;
	// ���������������½ڵ� 
	p->next = spare;
	if(spare->next) { spare->next->pre = spare; }
} 


// ��ǰ�����ӡ�ڴ������ 
void print_table(){
	cout << "��" << c << "��\n======================================================================\n";
	cout << "�ڴ���\t��ʼ��ַ\t��ֹ��ַ\t�ڴ��С\t�ѷ���\n";
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


// �����Ӧ�㷨 
void first_fit(){
	// ���������в�Ϊ�� 
	while(!work.empty()){
		print_table();
		pair<int, int> cur = work.front();
		// ��Ϊ -1��˵������ 
		if(cur.second == -1){
			free(cur.first);
		} else { // �����¿ռ� 
			alloc(cur);
		}
		work.pop_front();
		c++;
	}
	print_table();
}

bool sort_block(block* s1, block* s2){
	if(s1->state != s2->state){
		return s1->state < s2->state; // 1 Ϊ��ռ�ã����ȼ��ܵ� 
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
	// ���������в�Ϊ�� 
	while(!work.empty()){
		sort();
		print_table();
		pair<int, int> cur = work.front();
		// ��Ϊ -1��˵������ 
		if(cur.second == -1){
			free(cur.first);
		} else { // �����¿ռ� 
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
	cout << "��ѡ���㷨 (1 - �����Ӧ�㷨  or  2 - �����Ӧ�㷨): ";
	int num;
	cin >> num;
	run(num); 
    return 0;
}
