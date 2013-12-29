/*
 * Answer.h
 *
 *  Created on: 15-dec.-2013
 *      Author: arno
 */

#include "Path.h"

#ifndef ANSWER_H_
#define ANSWER_H_
class Answer {
public:
	Answer(const std::string& answer, const Path& p) :
			path(p), answer_(answer) {

	}
	~Answer() {
	}

	std::string getAnswer() const {
		return answer_;
	}

	void setAnswer(std::string& answer) {
		answer_ = answer;
	}

	Path path;

private:
	std::string answer_;
};

#endif /* ANSWER_H_ */
