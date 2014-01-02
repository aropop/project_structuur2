/*
 * editor.cpp
 *
 *  Created on: 10-nov.-2013
 *      Author: arno
 */

#include "WebApplication.h"
#include "AdminApplication.h"
#include <iostream>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>

Wt::WApplication *createApplication(const Wt::WEnvironment& env) {
	std::string path(env.deploymentPath());
	std::cout << path;
	if (path.compare("/admin") == 0) {
		return new AdminApplication(env);
	}
	return new WebApplication(env);
}

int main(int argc, char * args[]) {
	char *custom_argv[] = { "Enquête", "--docroot=/usr/share/Wt",
			"--http-address=127.0.0.1", "--http-port=8080" };
	try {
		Wt::WServer server(custom_argv[0]);

		server.setServerConfiguration(4, custom_argv, WTHTTP_CONFIGURATION);

		server.addEntryPoint(Wt::Application,
				boost::bind(&createApplication, _1), "/admin");

		server.addEntryPoint(Wt::Application,
				boost::bind(&createApplication, _1), "");

		if (server.start()) {
			Wt::WServer::waitForShutdown();
			server.stop();
		}

	} catch (Wt::WServer::Exception& e) {
		std::cerr << e.what() << std::endl;
	} catch (std::exception &e) {
		std::cerr << "exception: " << e.what() << std::endl;
	}

}
