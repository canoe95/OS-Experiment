#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <deque>
#include <string>
#include <algorithm>

#define MEM_SIZE 4 // 内存块个数 
#define ORDER_SIZE 320 // 作业指令总数 
#define PAGE_SIZE 10 // 每个页面能存放的指令数 

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

// 内存块
deque<page> queue;
// 指令执行序列
vector<int> order(ORDER_SIZE);


// 打印指令序列
void print_order() {
	for (int i = 0; i < ORDER_SIZE; i++) {
		cout << i << ". " << order[i] << "-" << order[i] / PAGE_SIZE << "    \t";
		if ((i + 1) % 5 == 0) {
			cout << endl;
		}
	}
	cout << "\n\n";
}

// 记录缺页次数
int lack = 0;

// 随机初始化指令序列
// 保证一半的指令是连续的，四分之一在上条指令之前，四分之一在上条指令之后
void init_order() {
	lack = 0;
	int i = 1; // 记录已分配的指令条数
	int cur = rand() % ORDER_SIZE;
	order[0] = cur;
	while (i < ORDER_SIZE) {
		// 连续
		order[i] = ++cur;
		i++;
		if (i >= ORDER_SIZE) {
			break;
		}
		// 前驱
		order[i] = rand() % (cur) + 0;
		cur = order[i];
		i++;
		if (i >= ORDER_SIZE) {
			break;
		}
		order[i] = ++cur; // 连续
		i++;
		// 后继
		if (i >= ORDER_SIZE) {
			break;
		}
		order[i] = rand() % (ORDER_SIZE - cur + 2) + cur + 1;
		cur = order[i];
		i++;
	}
	print_order();
}


// 获取指令序列的页号
int get_page(int order) {
	return order / PAGE_SIZE;
}

// 判断指令序列是否在内存中
int in_memory(int index) {
	int id = get_page(order[index]);
	for (int i = 0; i < queue.size(); i++) {
		if (id == queue[i].id) {
			return i;
		}
	}
	return -1;
}

// 判断内存块是否占满
bool full() {
	return queue.size() == MEM_SIZE;
}

// 尾插页面到内存块
void add_page(page p) {
	queue.push_back(p);
}

// 将块号为 target 的内存块页面置换为 p
void swap_page(page p, int target) {
	queue[target] = p;
}



// 先进先出，返回置换的块号
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

// 除了下标为 except 的页，lru 加一
void lru_increase(int except) {
	for (int i = 0; i < queue.size(); i++) {
		if (i != except) {
			queue[i].lru_time++;
		}
	}
}

// 最近最久未使用，返回置换的块号
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


// 除了下标为 except 的页，opt 减一
void opt_decrease(int except) {
	for (int i = 0; i < queue.size(); i++) {
		if (i != except) {
			queue[i].opt_time--;
		}
	}
}

int opt(int seq, int index) {
	if (seq >= 0) {
		// 重置被选中页的 opt 时间
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
	// 初始化新 page
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

// 执行
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

	cout << index << " - 指令 " << order[index] << "#，页号 " << id << "，";
	// 打印缺页情况
	if (seq >= 0) {
		cout << "不发生缺页，页内存地址为 " << &queue[seq] << "，占用内存块 " << seq << endl;
	} else {
		cout << "发生缺页，将页 " << id << " 调入内存块 " << target << endl;
	}

	// 打印内存块情况
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

// 判断是否继续死循环
bool flag = true;

// 打印统计信息
void print_statistics() {
	cout << "\n调页次数: " << lack << "\t缺页率: " << (double)lack / ORDER_SIZE << "\n\n\n\n";
	system("pause");
}

// 运行算法执行指令序列
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

// 选择算法
int select() {
	int algorithm;
	cout << endl <<  "请选择一种调页算法\n\n";
	cout << "1.OPT 最佳适应\n2.FIFO 先进先出\n3.LRU 最近最久未使用\n\n";
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

