/*
 * Parser.h
 *
 *  Created on: 14-nov.-2013
 *      Author: arno
 */

#include <iostream>
#include "QuestionList.h"
#include "AnswerSet.h"

#ifndef PARSER_H_
#define PARSER_H_

class Parser {
public:
	Parser(std::istream * in, std::ostream * out, QuestionList * ql);
	Parser(std::istream * in, std::ostream * out, QuestionList * ql, const std::string & answer_fn);
	~Parser();
	void parse_next();

private:
	std::string message_;
	std::istream * in_;
	std::ostream * out_;
	QuestionList * ql_;
	std::string answer_fn_;
	QuestionList::QLiterator current_it_;
	AnswerSet answers_;


	enum ParserCode {
		EXIT, WRONG_MESSAGE, CORRECT, TEST
	};

	ParserCode parser_code_;
	int current_amount_of_answers_;

	std::string * prompt_for_choices();
	std::string prompt_for_new_question_string(Path& index);

	void parse_dispatch_editor();
	void parse_dispatch_tester(bool save_answers, const std::string & file_name);
	bool prompt_save();

	void getToNextChar(std::stringstream& ss);

	inline void reset_parser_code();

	bool special_test_command(const std::string& command) const;
	void print_add_text(const std::string& question, Path position)const;
	void print_out_of_bounds(Path index) const;
};

#endif /* PARSER_H_ */
