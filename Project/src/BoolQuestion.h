/*
 * BoolQuestion.h
 *
 *  Created on: 1-dec.-2013
 *      Author: arno
 */

#ifndef BOOLQUESTION_H_
#define BOOLQUESTION_H_

#include "Question.h"

class BoolQuestion: public Question {
public:
	BoolQuestion();
	BoolQuestion(Path id, std::string& question_string);
	~BoolQuestion();
	static bool accepts_character(char ch);
	bool accepts_answer(std::string& answer) const;
	std::string get_string();
	std::string get_question_file_string();
};

#endif /* BOOLQUESTION_H_ */
