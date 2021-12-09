//#include <iostream>
#include <pybind11/pybind11.h>
#include "LeapListener.cpp"

namespace py = pybind11;

static Controller* controller;
static LeapListener* listener;

LeapListener* test() {
	// Create a sample listener and controller
	controller = new Controller();
	listener = new LeapListener();

	// Have the sample listener receive events from the controller
	controller->addListener(*listener);
	
	return listener;
}

int add(int i, int j) {
	return i + j;
}

// "main" in the next line must be the same as the filename (i.e. filename must be main.cpp)
PYBIND11_MODULE(main, m) {
	m.doc() = "Example plugin";
	m.def("add", &add, "A function which adds two numbers");
	m.def("test", &test, "test()");
	py::class_<LeapListener>(m, "LeapListener")
		.def("getNumFingersExtended", &LeapListener::getNumFingersExtended)
		.def("getMovement", &LeapListener::getMovement);
}
