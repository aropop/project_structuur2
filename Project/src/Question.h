/*
 * Question.h
 *
 *  Created on: 10-nov.-2013
 *      Author: arno
 */
#include <iostream>
#include <sstream>
#include <Wt/WContainerWidget>
#include "Path.h"

#ifndef QUESTION_H_
#define QUESTION_H_



class Question {
public:

	enum QuestionType {
		CHOICE,
		TEXT,
		BOOL,
		SCALE,
		GROUP
	};


	Question(Path id, std::string& question);
	Question();
	virtual ~Question();

	void set_question_string(std::string& new_question);

	virtual std::string get_question_file_string() const;
	virtual std::string get_string() const;
	virtual std::string get_string(int spacing) const;
	virtual std::string get_asking_string() const;
	virtual std::string get_ok_string(bool ok, int level=0) const;
	virtual Wt::WContainerWidget* getWidget() const;

	virtual void set_answers(std::string * answers, int amount);

	static std::string get_type_string(QuestionType type);

	virtual bool accepts_answer(std::string& answer) const;

	virtual Path increase_id(int level = 0);
	virtual Path decrease_id(int level = 0);

	Path getId() const {
		return id_;
	}

	void setId(Path id) {
		id_ = id;
	}

	QuestionType getType() const {
		return type_;
	}


	std::string question_string;

	bool copied;
protected:
	Path id_;
	QuestionType type_;


};



std::ostream& operator<< (std::ostream& out, Question& q);

#endif /* QUESTION_H_ */
