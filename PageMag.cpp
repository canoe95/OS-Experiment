#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <deque>
#include <string>
#include <algorithm>

#define MEM_SIZE 4 // �ڴ����� 
#define ORDER_SIZE 320 // ��ҵָ������ 
#define PAGE_SIZE 10 // ÿ��ҳ���ܴ�ŵ�ָ���� 

#define OPT 1
#define FIFO 2
#define LRU 3

struct page {
	int id;
	int opt_time;
	int lru_time;

	page() {}

	page(int id) {
		this->id = id;
		opt_time = 0;
		lru_time = 0;
	}

	page(int id, int opt_time) {
		this->id = id;
		this->opt_time = opt_time;
		lru_time = 0;
	}
};

// �ڴ��
deque<page> queue;
// ָ��ִ������
vector<int> order(ORDER_SIZE);


// ��ӡָ������
void print_order() {
	for (int i = 0; i < ORDER_SIZE; i++) {
		cout << i << ". " << order[i] << "-" << order[i] / PAGE_SIZE << "    \t";
		if ((i + 1) % 5 == 0) {
			cout << endl;
		}
	}
	cout << "\n\n";
}

// ��¼ȱҳ����
int lack = 0;

// �����ʼ��ָ������
// ��֤һ���ָ���������ģ��ķ�֮һ������ָ��֮ǰ���ķ�֮һ������ָ��֮��
void init_order() {
	lack = 0;
	int i = 1; // ��¼�ѷ����ָ������
	int cur = rand() % ORDER_SIZE;
	order[0] = cur;
	while (i < ORDER_SIZE) {
		// ����
		order[i] = ++cur;
		i++;
		if (i >= ORDER_SIZE) {
			break;
		}
		// ǰ��
		order[i] = rand() % (cur) + 0;
		cur = order[i];
		i++;
		if (i >= ORDER_SIZE) {
			break;
		}
		order[i] = ++cur; // ����
		i++;
		// ���
		if (i >= ORDER_SIZE) {
			break;
		}
		order[i] = rand() % (ORDER_SIZE - cur + 2) + cur + 1;
		cur = order[i];
		i++;
	}
	print_order();
}


// ��ȡָ�����е�ҳ��
int get_page(int order) {
	return order / PAGE_SIZE;
}

// �ж�ָ�������Ƿ����ڴ���
int in_memory(int index) {
	int id = get_page(order[index]);
	for (int i = 0; i < queue.size(); i++) {
		if (id == queue[i].id) {
			return i;
		}
	}
	return -1;
}

// �ж��ڴ���Ƿ�ռ��
bool full() {
	return queue.size() == MEM_SIZE;
}

// β��ҳ�浽�ڴ��
void add_page(page p) {
	queue.push_back(p);
}

// �����Ϊ target ���ڴ��ҳ���û�Ϊ p
void swap_page(page p, int target) {
	queue[target] = p;
}



// �Ƚ��ȳ��������û��Ŀ��
int fifo(int seq, int index) {
	if (seq >= 0) {
		return -1;
	}
	lack++;
	int id = get_page(order[index]);
	page p = page(id);
	if (full()) {
		queue.pop_front();
	}
	add_page(p);
	return queue.size() - 1;
}

// �����±�Ϊ except ��ҳ��lru ��һ
void lru_increase(int except) {
	for (int i = 0; i < queue.size(); i++) {
		if (i != except) {
			queue[i].lru_time++;
		}
	}
}

// ������δʹ�ã������û��Ŀ��
int lru(int seq, int index) {
	if (seq >= 0) {
		queue[seq].lru_time = 0;
		lru_increase(seq);
		return -1;
	}
	lack++;
	int id = get_page(order[index]);
	page p = page(id);
	if (full()) {
		int max = -1, target = -1;
		for (int i = 0; i < queue.size(); i++) {
			if (queue[i].lru_time > max) {
				max = queue[i].lru_time;
				target = i;
			}
		}
		swap_page(p, target);
		return target;
	}
	add_page(p);
	lru_increase(queue.size() - 1);
	return queue.size() - 1;
}


// �����±�Ϊ except ��ҳ��opt ��һ
void opt_decrease(int except) {
	for (int i = 0; i < queue.size(); i++) {
		if (i != except) {
			queue[i].opt_time--;
		}
	}
}

int opt(int seq, int index) {
	if (seq >= 0) {
		// ���ñ�ѡ��ҳ�� opt ʱ��
		queue[seq].opt_time = ORDER_SIZE;
		for (int i = index + 1; i < ORDER_SIZE; i++) {
			if (get_page(order[i]) == queue[seq].id) {
				queue[seq].opt_time = i - index;
				break;
			}
		}
		opt_decrease(seq);
		return -1;
	}
	lack++;
	int id = get_page(order[index]);
	// ��ʼ���� page
	page p = page(id, ORDER_SIZE);
	for (int i = index + 1; i < ORDER_SIZE; i++) {
		if (id == get_page(order[i])) {
			p.opt_time = i - index;
			break;
		}
	}
	if (full()) {
		int max = -1, target = -1;
		for (int i = 0; i < queue.size(); i++) {
			if (queue[i].opt_time > max) {
				max = queue[i].opt_time;
				target = i;
			}
		}
		swap_page(p, target);
		opt_decrease(target);
		return target;
	}
	add_page(p);
	opt_decrease(queue.size() - 1);
	return queue.size() - 1;
}

// ִ��
void execute(int index, int algorithm) {
	cout << endl;
	int id = get_page(order[index]);
	int seq = in_memory(index);

	int target = -1;
	switch (algorithm) {
		case OPT:
			target = opt(seq, index);
			break;
		case FIFO:
			target = fifo(seq, index);
			break;
		case LRU:
			target = lru(seq, index);
			break;
	}

	cout << index << " - ָ�� " << order[index] << "#��ҳ�� " << id << "��";
	// ��ӡȱҳ���
	if (seq >= 0) {
		cout << "������ȱҳ��ҳ�ڴ��ַΪ " << &queue[seq] << "��ռ���ڴ�� " << seq << endl;
	} else {
		cout << "����ȱҳ����ҳ " << id << " �����ڴ�� " << target << endl;
	}

	// ��ӡ�ڴ�����
	for (int i = 0; i < queue.size(); i++) {
		cout << i << ": " << queue[i].id;
		switch (algorithm) {
			case OPT:
				cout << "\tOPT: " << queue[i].opt_time;
				break;
			case LRU:
				cout << "\tLRU: " << queue[i].lru_time;
				break;
		}
		cout << endl;
	}
	cout << endl;
}

// �ж��Ƿ������ѭ��
bool flag = true;

// ��ӡͳ����Ϣ
void print_statistics() {
	cout << "\n��ҳ����: " << lack << "\tȱҳ��: " << (double)lack / ORDER_SIZE << "\n\n\n\n";
	system("pause");
}

// �����㷨ִ��ָ������
void run(int algorithm) {
	if (algorithm != OPT && algorithm != FIFO && algorithm != LRU) {
		flag = false;
		return;
	}
	for (int i = 0; i < ORDER_SIZE; i++) {
		execute(i, algorithm);
	}
	print_statistics();
}

// ѡ���㷨
int select() {
	int algorithm;
	cout << endl <<  "��ѡ��һ�ֵ�ҳ�㷨\n\n";
	cout << "1.OPT �����Ӧ\n2.FIFO �Ƚ��ȳ�\n3.LRU ������δʹ��\n\n";
	cin >> algorithm;
	return algorithm;
}


int main() {
	while (flag) {
		init_order();
		run(select());
	}
	return 0;
}

