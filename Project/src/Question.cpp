/*
 * Question.cpp
 *
 *  Created on: 10-nov.-2013
 *      Author: arno
 */

#include "Question.h"
#include <iostream>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WBreak>

//Constructor meestal gebruikt voor type
Question::Question(Path id, std::string& question) :
		question_string(question), id_(id), type_(TEXT) {

}

Question::Question() :
		id_(Path(0)) {

}

//Destructor
Question::~Question() {

}
//verander de question string
void Question::set_question_string(std::string& new_question) {
	question_string = new_question;
}

//hulp functies voor het aanpassen van het id
Path Question::increase_id(int level) {
	Path store;
	//store numbers on other levels
	for (int j = 0; j < level; ++j) {
		store.push_number(id_.pop_front_number());
	}
	int i(id_.pop_front_number());
	++i;
	id_.push_front_number(i);
	for (int j = 0; j < level; ++j) {
		id_.push_front_number(store.pop_number());
	}
	return id_;
}

Path Question::decrease_id(int level) {
	Path store;
	//store numbers on other levels
	for (int j = 0; j < level; ++j) {
		store.push_number(id_.pop_front_number());
	}
	int i(id_.pop_front_number());
	--i;
	id_.push_front_number(i);
	for (int j = 0; j < level; ++j) {
		id_.push_front_number(store.pop_number());
	}
	return id_;
}

//zorgt voor een gemakkelijk output
std::ostream& operator<<(std::ostream& out, Question& q) {
	out << q.get_string();
	return out;
}

std::string Question::get_question_file_string() const {
	return get_string();
}

std::string Question::get_string() const {
	std::stringstream ss;
	ss << id_.toString() << " " << Question::get_type_string(type_) << " "
			<< question_string << std::endl;
	return ss.str();
}
std::string Question::get_string(int spacing) const {
	std::stringstream ss;
	for (int i = 0; i < spacing; ++i) {
		ss << "     ";
	}
	ss << get_string();
	return ss.str();
}

void Question::set_answers(std::string * answers, int amount) {
	throw std::string("Kan geen antwoorden toevoegen voor dit soort vraag!");
}
//statische functie die de string voor het questiontype terug geeft
std::string Question::get_type_string(Question::QuestionType type) {
	std::string return_string;
	switch (type) {
	case Question::CHOICE:
		return_string = std::string("CHOICE");
		break;
	case Question::TEXT:
		return_string = std::string("TEXT");
		break;
	case Question::BOOL:
		return_string = std::string("BOOL");
		break;
	case Question::SCALE:
		return_string = std::string("SCALE");
		break;
	case Question::GROUP:
		return_string = std::string("GROUP");
		break;
	}
	return return_string;
}

bool Question::accepts_answer(std::string& answer) const {
	return true;
}

std::string Question::get_asking_string() const {
	return get_string();
}

std::string Question::get_ok_string(bool ok, int level) const {
	std::stringstream ss;
	for (int i = 0; i < level; ++i) {
		ss << "     ";
	}
	ss << id_.toString() << " ";
	if (ok) {
		ss << "OK";
	} else {
		ss << "NOK";
	}
	ss << " " << question_string << std::endl;
	return ss.str();
}

Wt::WContainerWidget* Question::getWidget() const {
	Wt::WContainerWidget* ret(new Wt::WContainerWidget());
	ret->addWidget(new Wt::WText(question_string));
	ret->addWidget(new Wt::WBreak());
	//Wt::WText* hidden = new Wt::WText(id_.toString());
	//hidden->setObjectName("path");
	//ret->addWidget(hidden);
	//hidden->hide();
	//Wt::WLineEdit* id = new Wt::WLineEdit(ret);
	//id->setObjectName("answer");
	return ret;
}


