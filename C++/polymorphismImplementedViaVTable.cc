#include <bits/stdc++.h>

using namespace std;

typedef void (ExecFn)(void*);

struct VTable {
	ExecFn* executeHandler;
};

class BaseFsm {
	static VTable vtable;
public:
	int counter;
	VTable* vPtr;

	BaseFsm() : counter(0), vPtr(&vtable) {}

	static void executeHandler(void*) {
		cout << "invoking BaseFsm::executeHandler()" << endl;
	}
};

class App1Fsm : public BaseFsm {
	static VTable vtable;
public:
	App1Fsm() {
		vPtr = &vtable;
	}

	static void executeHandler(void* self) {
		((App1Fsm*)self)->counter ++;
	}
};

class App2Fsm : public BaseFsm {
	static VTable vtable;
public:
	App2Fsm() {
		vPtr = &vtable;
	}

	static void executeHandler(void* self) {
		((App2Fsm*)self)->counter ++;
	}
};

// Create the static VTable instances
VTable BaseFsm::vtable {
  (ExecFn*)&BaseFsm::executeHandler
};
VTable App1Fsm::vtable {
  (ExecFn*)&App1Fsm::executeHandler
};
VTable App2Fsm::vtable {
  (ExecFn*)&App2Fsm::executeHandler
};

int main(int argc, char* argv[]) {
	if (argc < 2) exit(1);

	BaseFsm* app = new App1Fsm;
	for(int i = 0; i < atoi(argv[1]); i++)
		app->vPtr->executeHandler(app);
	cout << "App1Fsm::executeHandler() " << app->counter << " times" << endl;

	app = new App2Fsm;
	for(int i = 0; i < atoi(argv[1]); i++)
		app->vPtr->executeHandler(app);
	cout << "App2Fsm::executeHandler() " << app->counter << " times" << endl;
}
