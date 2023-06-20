---
title: 操作系统实验
date: 2022-11-20
---

## PCB 调度模拟

> 优先级调度算法

头文件引入；结构体及内函数定义

```c
#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <deque>
#include <algorithm>
#include <string> 

struct PCB {
	int id; // 标识 
	int priority; // 优先级 
	int cpu_time; // 已占用的 cpu 时间 
	int all_time; // 还需要占用的 cpu 时间 
	int start_block; // 经过多少个时间片后进入阻塞态 
	int block_time; //  阻塞的时间片 
	int state; // 进程状态，执行完 -1 、阻塞 0、就绪 1
	
	PCB() {}
	
	PCB(int i, int p, int c, int a, int sb, int bt, int s){
		id = i;
		priority = p;
		cpu_time = c;
		all_time = a;
		start_block = sb;
		block_time = bt;
		state = s;
	} 
	
	void print_priority(){
		cout << id << '[' << priority << ']'; 
	}
	
	void print(){
		cout << id << '\t'
			 << priority << "\t\t"
			 << cpu_time << "\t\t"
			 << all_time << "\t\t"
			 << start_block << "\t\t"
			 << block_time << "\t\t";
		if(state == -1){
			cout << "end\n";
		} else if(state == 0){
			cout << "block\n";
		} else if(state == 1){
			cout << "ready\n";
		}
	}
};
```

定义全局变量

```c
deque<PCB> queue; // 执行顺序表 
vector<int> index; // 下标映射，因为表需要从0按序打印，而队列是根据优先级排列，即 index[0] 是 PCB-0 在 queue 中的下标 
int ready_count = 0; // 就绪进程数
int block_count = 0; // 阻塞进程数
int end_count = 0; // 完成进程数
int slice_count = 0; // 总时间片使用数
```

初始化

```c
// 初始化 table 
void init(){
	PCB p0(0,9,2,1,0,0,0), p1(1,38,3,0,-1,0,-1), p2(2,30,0,6,-1,0,1),
		p3(3,29,0,3,-1,0,1), p4(4,0,0,4,-1,0,1);
		
	queue.push_back(p0);
	queue.push_back(p1);
	queue.push_back(p2);
	queue.push_back(p3);
	queue.push_back(p4);
	
	for(int i = 0; i < queue.size(); i++){
		index.push_back(i);
	}
	
	set_sequence();
	update_index();
}
```

优先级排序

```c
// PCB 排序 
bool PCB_sort(PCB p1, PCB p2){
	if(p1.state != p2.state){
		return p1.state > p2.state;
	}
	return p1.priority > p2.priority;
}


// 调整 queue 中 PCB 优先顺序 
void sort_queue(){
	sort(queue.begin(), queue.end(), PCB_sort);
	ready_count = 0; block_count = 0; end_count = 0;
	for(int i = 0; i < queue.size(); i++){
		if(queue[i].state == 1){
			ready_count++;
		} else if(queue[i].state == 0){
			block_count++;
		} else if(queue[i].state == -1){
			end_count++;
		}		
	}
}
```

主逻辑实现

- 更新优先级队列，按照优先级排序
  - 就绪 > 阻塞 > 终止
  - 同一状态下，优先级权重高的优先
- 进程状态更新
  - 当前运行进程状态更新
  - 就绪进程状态更新
  - 阻塞进程状态更新
- 实现 PCB ID 和其在优先队列中下标的映射：为了方便按 ID 顺序输出

```c
// 设置执行队列顺序，若第一个进程也执行完，说明所有进程执行完，返回 false 
int set_sequence(){
	sort_queue();
	PCB cur = queue.front();
	if(cur.state == -1){
		return 0;
	}
	return 1;
}
	
void use(PCB& p){
	p.priority -= 3;
	p.cpu_time++;
	p.all_time--;
	// 若执行完，直接置为 -1 ，不进行后续判断 
	if(p.all_time <= 0){
		p.state = -1;
	} else { // 若未执行完，判断是否阻塞 
		if(p.start_block > 0){
			p.start_block--;
		}
		if(p.start_block == 0){
			p.state = 0;
		}
	}	
}

void ready_use(PCB& p){
	p.priority++;
}

void block_use(PCB& p){
	p.block_time--;
	if(p.block_time <= 0){
		p.state = 1;
	}
}


void update_index(){
	for(int i = 0; i < queue.size(); i++){
		//cout << queue[i].id << endl;
		index[queue[i].id] = i;
	}
}


void use(){
	use(queue[0]);
	for(int i = 1; i < ready_count; i++){
		ready_use(queue[i]);
	}
	for(int i = ready_count; i < ready_count + block_count; i++){
		block_use(queue[i]);
	}
}
```

打印输出

```c
void print_colume(){
	cout << "ID\tPriority\tCPU Time\tAll Time\tStart Block\tBlock Time\tState" << endl;
	cout << "---------------------------------------------------------------------------------------------\n";
}

// 打印当前所有进程状态 
void print_table(int flag){
	cout << "System CPU Time: " << slice_count << endl; // 使用时间片 
	
	cout << "Running Process: ";
	if(flag){
		queue[0].print_priority(); 		// 本次运行 PCB 
	}
	cout << endl;
	
	// 就绪队列 
	cout << "Ready Queue: ";  
	for(int i = 1; i < ready_count; i++){
		queue[i].print_priority();
		cout << "->";
	}
	cout << "null" << endl;
	
	// 阻塞队列 
	cout << "Block Queue: ";  
	for(int i = ready_count; i < ready_count + block_count; i++){
		queue[i].print_priority();
		cout << "->";
	}
	cout << "null" << endl;

	// 完成队列 	
	cout << "End Queue: ";  
	for(int i = ready_count + block_count; i < queue.size(); i++){
		queue[i].print_priority();
		cout << "->";
	}
	cout << "null" << endl;
	
	cout << "=============================================================================================\n";
	print_colume();
	for(int i = 0; i < queue.size(); i++){
		//cout << index[i] << endl;
		queue[index[i]].print();
	}
	cout << "=============================================================================================\n";
} 
```

运行一个时间片：返回是否全部运行结束

```c
int run(){
	int flag = set_sequence();	
	update_index();
	print_table(flag);
	if(flag) { use(); slice_count++; }
	return flag;
}
```

主函数

```c
int main(){
	init();
	while(run()) {}
    return 0;
}
```

## 实存块分配 / 回收模拟

> 最快适应算法 & 最佳适应算法

定义结构体，使用双向链表链接内存块

```c
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
```

定义全局变量

```c
int c; // 操作轮次 
block* table; // 内存块表 
deque<pair<int, int> > work; // 工作序列 
```

初始化

```c
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
```

删除节点

```c
// 删除内存块指针 
void delete_spare(block* s){
	delete(s);
	s = NULL;
	delete(s);
}
```

合并两个节点：将后者合并到前者，删除后者

```c
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
```

分配和释放节点

```c
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
```

就是从前往后遍历，直接分配和释放

```c
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
```

按内存块大小从大到小排序空闲内存块，同时将已分配内存块放在链表末尾

```c
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
```

最佳适应算法，就是在最快适应算法的基础上加了一条排序指令

```c
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
```

运行一整个工作序列，接收参数为算法选择

```c
void run(int algorithm){
	switch(algorithm){
		case 1: first_fit(); break;
		case 2: best_fit(); break;
		default: break;
	}
}
```

主函数

```c
int main(){
	init();
	cout << "请选择算法 (1 - 最快适应算法  or  2 - 最佳适应算法): ";
	int num;
	cin >> num;
	run(num); 
    return 0;
}
```

## 虚存调页模拟

> OPT / FIFO / LRU

引入库，宏定义算法

```c
#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <deque>
#include <string>
#include <algorithm>

#define OPT 1
#define FIFO 2
#define LRU 3
```

定义虚存所用内存块个数；定义指令序列总数；定义每个页所能存放的指令序列数

```c
#define MEM_SIZE 4 // 内存块个数 
#define ORDER_SIZE 320 // 作业指令总数 
#define PAGE_SIZE 10 // 每个页面能存放的指令数 
```

定义 page 结构体，id 为页号，opt_time 和 lru_time 分别用于 OPT 算法和 LRU 算法优先级判定

```c
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
```

用大小为 MEM_SIZE 的 deque 代表虚存所用的四个内存块

用大小为 ORDER_SIZE 的 vector 存储指令序列

```c
// 内存块
deque<page> queue;
// 指令执行序列
vector<int> order(ORDER_SIZE);
```

生成指令序列

- 有一半的指令连续
- 有四分之一的指令序号小于前驱
- 有四分之一的指令序号大于前驱

使用 rand() 函数生成随机数，取余 ORDER_SIZE 得到序列号在区间`[1, ORDER_SIZE]`内

```c
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
```

打印指令序列，就是按格式打印 order 数组

``` 
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
```

获取指令页号，就是序列号除以每页指令大小

```c
// 获取指令序列的页号
int get_page(int order) {
	return order / PAGE_SIZE;
}
```

判断指令序列是否在内存块中，传入参数为序列在 order 数组中的下标

```c
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
```

判断内存块是否占满

```c
// 判断内存块是否占满
bool full() {
	return queue.size() == MEM_SIZE;
}
```

尾插页面到内存块中

```c
// 尾插页面到内存块
void add_page(page p) {
	queue.push_back(p);
}
```

将块编号为 target 的内存块所存页面置换为页 p

```c
// 将块号为 target 的内存块页面置换为 p
void swap_page(page p, int target) {
	queue[target] = p;
}
```

维护全局变量`int lack`，记录缺页次数，用于统计缺页率

> 先进先出算法：调页时换出最先当前内存中进入内存的页面

入参

- seq：传入当前使用的内存块编号，若小于 0，表示发生缺页
- index：当前执行指令在 order 数组中的下标

若未发生缺页，直接返回即可

若发生缺页，若内存块占满，pop_front() 后将当前页面尾插进内存，若未占满，直接尾插

```c
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
```

最近最久未使用算法：调页时换出最近最久未被使用的页面

传入当前使用页面的内存块编号，其余的内存块的 lru_time 均加一

- page.lru_time 表示该页面没被使用的时间

```c
// 除了下标为 except 的页，lru 加一
void lru_increase(int except) {
	for (int i = 0; i < queue.size(); i++) {
		if (i != except) {
			queue[i].lru_time++;
		}
	}
}
```

lru 算法

- seq：传入当前调用页面的内存块编号，小于 0 表示缺页
- index：当前指令序列的下标

当不缺页，令除当前使用的内存块页面以外所有页面 lru_time+1，然后返回

若缺页，首先 lack+1，若内存块全被占用，找到内存中 lru_time 最大的页面，将其换出，换成新的页面，新页面的 lru_time = 0，若内存未满，直接尾插，同时令其余页面的 lru_time+1

```c
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
```

最佳调页算法：调页时换出最久将不被使用的页面

在调入页面时，向指令序列后搜索当前页面再次被使用的时间（下标差），记录在 opt_time 中，每经过一轮，未被使用的页面其 opt_time-1，调页时，换出 opt_time 最大的页面

将除了下标为 except 的内存页面的 opt_time 加一

```c
// 除了下标为 except 的页，opt 减一
void opt_decrease(int except) {
	for (int i = 0; i < queue.size(); i++) {
		if (i != except) {
			queue[i].opt_time--;
		}
	}
}
```

opt 算法：多一个向后搜寻的过程，初始化为 MEM_SIZE，若未找到下次，opt_time 即为 MEM_SIZE

- 若内存中页面均无下次，OPT 退化为 FIFO 算法

```c
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
```

整合算法，执行指令

执行单条指令

```c
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
```

执行所有指令

```c
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
```

打印统计信息

```c
// 打印统计信息
void print_statistics() {
	cout << "\n调页次数: " << lack << "\t缺页率: " << (double)lack / ORDER_SIZE << "\n\n\n\n";
	system("pause");
}
```

获取用户输入，选择算法

```c
// 选择算法
int select() {
	int algorithm;
	cout << endl <<  "请选择一种调页算法\n\n";
	cout << "1.OPT 最佳适应\n2.FIFO 先进先出\n3.LRU 最近最久未使用\n\n";
	cin >> algorithm;
	return algorithm;
}
```

主函数

```c
// 判断是否继续死循环
bool flag = true;
int main() {
	while (flag) {
		init_order();
		run(select());
	}
	return 0;
}
```

## 文件系统模拟

> 树形文件系统
>

引入库，ctime 用于生成当前系统时间，WRITE / READ 表示文件权限，DIR / FILE 表示文件类型（文件夹和文本文件均属于文件），DIR_SIZE 为文件夹大小，单位为 MB

```c
#include <iostream>
using namespace std;
#include <vector>
#include <ctime>
#include <string>

#define WRITE 0
#define READ 1
#define EXEC 2

#define DIR 3
#define FILE 4

#define DIR_SIZE 4096
```

File 结构体，采用树的结构表示各个文件的层级关系，其中

- father 指针指向自己的父级目录
- children 数组指针记录自己的子级目录

```c
struct File {
	int type;
	string name;
	string time;
	File *father;
	int size;
	int permission;
	vector<File *> children;

	File() {}

	File(int type, string name, char *time, File *father) {
		this->type = type;
		this->name = name;
		this->time = time;
		this->time[this->time.length() - 1] = '\0';
		this->father = father;
		this->children = vector<File *>();
		this->permission = WRITE;
		this->size = type == DIR ? DIR_SIZE : 0;
	}
};
```

使用 cur 指针表示当前文件系统所在目录

```c
// 当前目录
static File *cur;

void init() {
	time_t now = time(0);
	File *root = new File(DIR, "home", ctime(&now), NULL);
	cur = root;
}
```

help，打印帮助文档

```c
void help() {
	cout << "format: format the file storage\
			\nmkdir: to create subdirectory\
			\nrmdir: to delete the subdirectory\
			\nls: display the dir\
			\ncd: change the dir\
			\ncreate : create the file\
			\nrm: delete file\
			\npwd: display the current dir path\
			\nexit: exit the system\n";
}
```

pwd，从当前目录递归，打印目录路径

```c
void pwd(File *file) {
	if (file->father != NULL) {
		pwd(file->father);
	}
	cout << "/" << file->name;
}
```

remove，递归删除当前 File 子树

```c
void remove(File *file) {
	for (int i = 0; i < (int)file->children.size(); i++) {
		remove(file->children[i]);
	}
	file->children.clear();
	delete file;
}
```

ls，展示当前目录子级文件

```c
void ls(File *file) {
	cout << "name\t\tsize\t\tlast write time\t\t\tpermission\n";
	for (int i = 0; i < (int)file->children.size(); i++) {
		File *f = file->children[i];
		cout << f->name << "\t\t" << f->size << "\t\t"
		     << f->time << "\t";
		switch (f->permission) {
			case WRITE:
				cout << "write\n";
				break;
			case READ:
				cout << "read\n";
				break;
			case EXEC:
				cout << "exec\n";
				break;
		}
	}
}
```

mkdir，create，创建文件，前者创建目录，后者创建文本

```c
void mkdir(File *file, string name) {
	for (int i = 0; i < (int)file->children.size(); i++) {
		if (file->children[i]->name == name) {
			cout << "failed to create, duplicate file name!\n";
			return;
		}
	}
	time_t now = time(0);
	File *son = new File(DIR, name, ctime(&now), file);
	file->children.push_back(son);
	cout << "folder created successfully!\n";
	pwd(son);

	cout << endl;
}

void create(File *file, string name) {
	for (int i = 0; i < (int)file->children.size(); i++) {
		if (file->children[i]->name == name) {
			cout << "failed to create, duplicate file name!\n";
			return;
		}
	}
	time_t now = time(0);
	File *son = new File(FILE, name, ctime(&now), file);
	file->children.push_back(son);
	cout << "file created successfully!\n";
	pwd(son);
	cout << endl;
}
```

rmdir，删除文件

```c
void rmdir(File *file, string name) {
	for (int i = 0; i < (int)file->children.size(); i++) {
		File *f = file->children[i];
		if (f->name == name) {
			pwd(f);
			cout << endl;
			remove(f);
			file->children.erase(file->children.begin() + i);
			cout << "folder deleted successfully!\n";
			return;
		}
	}
	cout << "no such dir or file, failed to delete!\n";
}
```

format，格式化文件系统

```c
void format() {
	File *root = cur;
	while (root->father != NULL) {
		root = root->father;
	}
	cur = root;
	for (int i = 0; i < (int)root->children.size(); i++) {
		remove(root->children[i]);
	}
	root->children.clear();
	pwd(cur);
	cout << "\nformated successfully!\n";
}
```

cd，切换目录

```c
void cd(File *file, string name) {
	if (name == ".") {
		return;
	}
	if (name == "..") {
		if (file->father) {
			cur = file->father;
		}
		return;
	}
	for (int i = 0; i < (int)file->children.size(); i++) {
		File *f = file->children[i];
		if (f->name == name) {
			if (f->type != DIR) {
				cout << name << " is not a dir, cd failed!\n";
				return;
			}
			cur = f;
			return;
		}
	}
	cout << "no such folder!\n";
}
```

exec，整合以上功能

- exit 使用一个全局布尔变量 flag 实现

```c
int flag = true;

void exec(vector<string> commands) {
	int n = commands.size();
	if (n > 2) {
		cout << "no such command! maybe u need input \"help\" to get help\n";
		return;
	}
	string first = commands[0];
	if (first == "help") {
		if (n > 1) {
			cout << "no such command! maybe u need input \"help\" to get help\n";
			return;
		}
		help();
	} else if (first == "ls") {
		if (n > 1) {
			cout << "no such command! maybe u need input \"help\" to get help\n";
			return;
		}
		ls(cur);
	} else if (first == "format") {
		if (n > 1) {
			cout << "no such command! maybe u need input \"help\" to get help\n";
			return;
		}
		format();
	} else if (first == "cd") {
		if (n != 2) {
			cout << "no such command! maybe u need input \"help\" to get help\n";
			return;
		}
		cd(cur, commands[1]);
	} else if (first == "mkdir") {
		if (n != 2) {
			cout << "no such command! maybe u need input \"help\" to get help\n";
			return;
		}
		mkdir(cur, commands[1]);
	} else if (first == "create") {
		if (n != 2) {
			cout << "no such command! maybe u need input \"help\" to get help\n";
			return;
		}
		create(cur, commands[1]);
	} else if (first == "rmdir") {
		if (n != 2) {
			cout << "no such command! maybe u need input \"help\" to get help\n";
			return;
		}
		rmdir(cur, commands[1]);
	} else if (first == "exit") {
		if (n > 1) {
			cout << "no such command! maybe u need input \"help\" to get help\n";
			return;
		}
		flag = false;
	} else {
		cout << "no such command! maybe u need input \"help\" to get help\n";
	}
}
```

获取用户输入

```c
void readline() {
	cout << "\nroot@localhost:";
	pwd(cur);
	cout << "$ ";
	string str;
	getline(cin, str, '\n');
	int start = 0, end = 0, count = 0;
	vector<string> commands;
	while (start < (int)str.length()) {
		while (str[start] == ' ') {
			start++;
		}
		end = start;
		while (end < (int)str.length() && str[end] != ' ') {
			end++;
		}
		string command = str.substr(start, end - start);
		commands.push_back(command);
		start = end + 1;
	}
	exec(commands);

}
```

主函数

```c
int main() {
	init();
	while (flag) {
		readline();
	}
}
```



