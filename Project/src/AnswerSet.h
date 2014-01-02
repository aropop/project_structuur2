/*
 * AnswerSet.h
 *
 *  Created on: 15-dec.-2013
 *      Author: arno
 */

#include <vector>
#include <iostream>
#include "Answer.h"
#include "Path.h"
#include "QuestionList.h"

#ifndef ANSWERSET_H_
#define ANSWERSET_H_

class AnswerSet {
public:
	AnswerSet(QuestionList* ql);
	AnswerSet();
	~AnswerSet();

	void write_to_file(const std::string & file, const std::string& uuid);

	void add(Answer& a);

	void list(std::ostream& out);

	bool fully_answered();

	void to_json(std::stringstream& ss);


private:
	std::vector<Answer> vect_;
	QuestionList* ql_;

	bool check_group_answered(Question* group, std::vector<Answer>::iterator);
};


#endif /* ANSWERSET_H_ */
