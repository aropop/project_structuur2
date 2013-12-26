/*
 * editor.cpp
 *
 *  Created on: 10-nov.-2013
 *      Author: arno
 */

#include "QuestionList.h"
#include "Parser.h"
#include "Path.h"
#include <iostream>

void print_help(std::ostream& out) {
	out << "Welkom bij de enquête tool." << std::endl
			<< "Gebruik: enqtool <specificatiebestand> <antwoordbestand>"
			<< std::endl << "Mogelijke commando's:" << std::endl
			<< "list, test, add, insert, group, ungroup, remove en edit." << std::endl;
}

int main(int argc, char * args[]) {

	if (argc == 2) {
		std::string fn(args[1]); //bestandsnaam nemen
		QuestionList ql = QuestionList(fn);
		Parser pa(&std::cin, &std::cout, &ql);
	} else if (argc == 3) {
		std::string quest_fn(args[1]);
		std::string ans_fn(args[2]);
		QuestionList ql = QuestionList(quest_fn);
		Parser pa(&std::cin, &std::cout, &ql, ans_fn);
	} else {
		print_help(std::cout);
	}
	return 0;
}
