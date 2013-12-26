/*
 * ScaleQuestion.cpp
 *
 *  Created on: 1-dec.-2013
 *      Author: arno
 */

#include "ScaleQuestion.h"
#include <stdlib.h>

ScaleQuestion::ScaleQuestion() {
	copied = false;
	type_ = SCALE;
}

ScaleQuestion::~ScaleQuestion() {
}

int ScaleQuestion::getMin() const {
	return min_;
}

ScaleQuestion::ScaleQuestion(Path id, std::string& question_string, int min,
		int max) :
		Question(id, question_string), min_(min), max_(max) {
	type_ = SCALE;
	copied = false;
}

bool ScaleQuestion::in_range(int possible_answer) const {
	return (min_ <= possible_answer) && (max_ >= possible_answer);
}

int ScaleQuestion::getMax() const {
	return max_;
}

std::string ScaleQuestion::get_question_file_string() const {
	std::stringstream ss;
	ss << id_.toString() << " " << get_type_string(type_) << " " << min_ << " "
			<< max_ << " " << question_string << std::endl;
	return ss.str();
}

std::string ScaleQuestion::get_asking_string() const{
	return get_question_file_string();
}

bool ScaleQuestion::accepts_answer(std::string& a) const {
	int value = atoi(a.c_str());
	return in_range(value);
}


