#include <iostream>
#include <format>
#if defined(_WIN32)
	#include <Windows.h>
#endif
#include "tree.h"


enum Bounds {
	LowerBound = 1, UpperBound = 100
};

int main() {

	#if defined(_WIN32)
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);
	#endif
	
	
	BTree<int> tree(2);
	
	
	for (int i = LowerBound; i <= UpperBound; i++) {
		tree.insert(i);
	}
	for (int i = LowerBound; i <= UpperBound; i++) {
		tree.insert(i);
	}
	for (int i = LowerBound; i <= UpperBound; i++) {
		tree.remove(i);
	}

	for (int i = LowerBound; i <= UpperBound/2; i++) {
		tree.remove(i);
	}
	std::cout << tree;

	return 0;
}