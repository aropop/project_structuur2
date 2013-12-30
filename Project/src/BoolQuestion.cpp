/*
 * BoolQuestion.cpp
 *
 *  Created on: 1-dec.-2013
 *      Author: arno
 */

#include "BoolQuestion.h"
#include <sstream>
#include <Wt/WRadioButton>
#include <Wt/WButtonGroup>
#include <Wt/WBreak>

BoolQuestion::BoolQuestion() :
		Question() {
	type_ = BOOL;
}

BoolQuestion::~BoolQuestion() {
}

BoolQuestion::BoolQuestion(Path id, std::string& question_string):Question(id, question_string) {
	type_ = BOOL;
}

bool BoolQuestion::accepts_character(char ch) {
	return (ch == 'j') || (ch == 'y') || (ch == '1') || (ch == 'n') || (ch == '0');
}

std::string BoolQuestion::get_string(){
	std::stringstream ss;
	ss << id_.toString() << " " << get_type_string(type_) << " " << question_string;
	return ss.str();
}
bool BoolQuestion::accepts_answer(std::string& answer) const {
	if(answer.length() != 1){ //if for "lazy" answer
		return false;
	}else{
		return accepts_character(answer.at(0));
	}
}

std::string BoolQuestion::get_question_file_string(){
	return get_string();
}

Wt::WContainerWidget* BoolQuestion::getWidget() const {
	Wt::WContainerWidget* ret (new Wt::WContainerWidget());
	Wt::WButtonGroup *group = new Wt::WButtonGroup(ret);

	ret->addWidget(new Wt::WText(question_string));
	new Wt::WBreak(ret);

	Wt::WRadioButton* yes (new Wt::WRadioButton("Ja", ret));
	yes->setObjectName("answer1");
	group->addButton(yes, 1);
	new Wt::WBreak(ret);

	Wt::WRadioButton* no (new Wt::WRadioButton("Nee", ret));
	no->setObjectName("answer0");
	group->addButton(no, 0);

	Wt::WText* hidden = new Wt::WText(id_.toString());
	hidden->hide();
	hidden->setObjectName("path");

	ret->addWidget(hidden);
	ret->addWidget(new Wt::WBreak());
	return ret;
}


