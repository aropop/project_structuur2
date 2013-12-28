/*
 * editor.cpp
 *
 *  Created on: 10-nov.-2013
 *      Author: arno
 */

#include "QuestionList.h"
#include "Parser.h"
#include "Path.h"
#include "WebApplication.h"
#include <iostream>
#include <Wt/WApplication>
#include "dirent.h"

void print_help(std::ostream& out) {
	out << "Welkom bij de enquête tool." << std::endl
			<< "Gebruik: enqtool <specificatiebestand> <antwoordbestand>"
			<< std::endl << "Mogelijke commando's:" << std::endl
			<< "list, test, add, insert, group, ungroup, remove en edit." << std::endl;
}

Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{

	return new WebApplication(env);
}

void debug(){
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir ("./src/")) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
		std::string fname(ent->d_name);
		if(fname.substr(fname.length() - 3, fname.length()).compare("ens") == 0){
			//Wt::WAnchor* anch(new Wt::WAnchor());
			//anch->setText(fname.substr(0, fname.length() - 3));
			//root()->addWidget(anch);
			std::cout << fname;
		}
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  //root()->addWidget(new Wt::WText("Interne fout"));
		std::cout << "fout";
	}
}



int main(int argc, char * args[]) {
	//debug();
	const char *custom_argv[] = {
				"hello",
				"--docroot=/usr/share/Wt",
				"--http-address=127.0.0.1",
				"--http-port=8080"
		};
	return WRun(4, (char **)(custom_argv), &createApplication);

}
