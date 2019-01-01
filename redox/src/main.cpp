#include <redox.h>
#include <iostream>


int main(int argc, const char* argv[]) {

	if (argc < 2) {
		std::cout << "Usage: redox.exe [App Folder]" << std::endl;
		return 1;
	}

	redox::Path appPath(argv[1]);
	if (!redox::io::is_directory(redox::io::absolute(appPath))) {
		std::cout << appPath << " does not exist or is not a folder." << std::endl;
		return 2;
	}

	try {
		redox::Application app(appPath);
		app.run();
	}
	catch (const redox::Exception& ex) {
		std::cout << "Exception ocurred: " << ex.what() << std::endl;
		return 3;
	}

	std::cin.ignore();
	return 0;
}