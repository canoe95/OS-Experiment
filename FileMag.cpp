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

void pwd(File *file) {
	if (file->father != NULL) {
		pwd(file->father);
	}
	cout << "/" << file->name;
}

void remove(File *file) {
	for (int i = 0; i < (int)file->children.size(); i++) {
		remove(file->children[i]);
	}
	file->children.clear();
	delete file;
}

// µ±Ç°Ä¿Â¼
static File *cur;

void init() {
	time_t now = time(0);
	File *root = new File(DIR, "home", ctime(&now), NULL);
	cur = root;
}


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

int flag = true;

void exec(vector<string> commands) {
	int n = commands.size();
//	cout << n << endl;
//	for (int i = 0; i < n; i++) {
//		cout << commands[i] << " ";
//	}
//	cout << endl;
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

void readline() {
	cout << "\nroot@localhost:";
	pwd(cur);
	cout << "$ ";
	string str;
	getline(cin, str, '\n');
	//cout << str.length() << endl;
	int start = 0, end = 0, count = 0;
	vector<string> commands;
	while (start < (int)str.length()) {
		while (str[start] == ' ') {
			start++;
		}
		end = start;
		//cout << start << " " << end << endl;
		while (end < (int)str.length() && str[end] != ' ') {
			end++;
		}
		//cout << endl << start << " " << end << endl;
		string command = str.substr(start, end - start);
		//cout << command;
		commands.push_back(command);
		start = end + 1;
	}
	exec(commands);

}

int main() {
	init();
	while (flag) {
		readline();
	}
}