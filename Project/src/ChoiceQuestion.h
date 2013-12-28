/*
 * ChoiceQuestion.h
 *
 *  Created on: 30-nov.-2013
 *      Author: arno
 */

#include "Question.h"

#ifndef CHOICEQUESTION_H_
#define CHOICEQUESTION_H_


class ChoiceQuestion : public Question{
public:
	ChoiceQuestion(Path id, std::string& question, std::string* answers, int amount_of_answers);
	ChoiceQuestion();
	~ChoiceQuestion();

	std::string get_question_file_string() const;
	std::string get_string() const;
	std::string get_asking_string() const;
	Wt::WContainerWidget* getWidget() const;

	bool accepts_answer(std::string& a) const;

	void set_answers(std::string* answers, int amount);

private:
	std::string * answers_;
	int amount_of_answers_;
	const int number_of_answers() const;
};

#endif /* CHOICEQUESTION_H_ */
