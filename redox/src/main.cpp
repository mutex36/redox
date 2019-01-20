#include <redox.h>
#include <iostream>


int main(int argc, const char* argv[]) {

	if (argc < 2) {
		std::cerr << "Usage: redox.exe [App Folder]";
		return 1;
	}

	redox::Path appPath(argv[1]);
	if (!redox::io::is_directory(redox::io::absolute(appPath))) {
		std::cerr << appPath << " does not exist or is not a folder.";
		return 2;
	}

	try {
		redox::Application app(appPath);
		app.run();
	}
	catch (const redox::Exception& ex) {
		std::cerr << "Exception ocurred: " << ex.what();
		return 3;
	}

	return 0;
}