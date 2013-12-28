/*
 * ChoiceQuestion.cpp
 *
 *  Created on: 30-nov.-2013
 *      Author: arno
 */

#include "ChoiceQuestion.h"
#include <stdlib.h>
#include <Wt/WRadioButton>
#include <Wt/WButtonGroup>
#include <Wt/WBreak>

ChoiceQuestion::ChoiceQuestion(Path id, std::string& question,
		std::string * answers, int amount_of_answers) :
		Question(id, question), answers_(answers), amount_of_answers_(
				amount_of_answers) {
	type_ = CHOICE;
	copied = false;
}
//voor initialisatie
ChoiceQuestion::ChoiceQuestion(void) :
		Question() {
	answers_ = NULL;
	copied = false;
}

ChoiceQuestion::~ChoiceQuestion() {
	if (answers_ != NULL && !copied) {
		delete[] answers_;
		//is niet gekopieerd dus niet zijn verantwoordelijkheid
	}
}

//veranderd de pointer van de antwoorden
void ChoiceQuestion::set_answers(std::string * answers, int amount) {
	if (type_ == CHOICE) {
		delete[] answers_;
		answers_ = answers;
		amount_of_answers_ = amount;
	} else {
		throw std::string(
				"Kan de antwoorden van een CHOICE vraag niet aanpassen!");
	}
}

bool ChoiceQuestion::accepts_answer(std::string& a) const {
	int value = atoi(a.c_str());
	return (0 < value) && (value <= amount_of_answers_);
}

std::string ChoiceQuestion::get_asking_string() const {
	std::stringstream ss;
	ss << get_string();
	for (int i = 0; i < amount_of_answers_; ++i) {
		ss << answers_[i] << std::endl;
	}
	return ss.str();
}

//returns the amount of answers for this question
const int ChoiceQuestion::number_of_answers() const {
	return amount_of_answers_;
}



std::string ChoiceQuestion::get_question_file_string() const {
	//string stream om gemakkelijk te werken
	std::stringstream file_stringstream;
	//standaard gedeelte invullen
	file_stringstream << id_ << " " << Question::get_type_string(type_) << " ";
	//specifiek gedeelte
	file_stringstream << number_of_answers() << " ";
	//vraag toevoegen
	file_stringstream << question_string << std::endl;

	//bij choice horen de antwoorden ook nog
	for (int i = 0; i < number_of_answers(); i++) {
		file_stringstream << answers_[i] << std::endl;
	}
	return file_stringstream.str();
}

std::string ChoiceQuestion::get_string() const {
	//string stream om gemakkelijk te werken
	std::stringstream file_stringstream;
	//standaard gedeelte invullen
	file_stringstream << id_ << " " << Question::get_type_string(type_) << " ";
	//specifiek gedeelte
	if (type_ == Question::CHOICE) {
		file_stringstream << number_of_answers() << " ";
	}
	//vraag toevoegen
	file_stringstream << question_string;
	file_stringstream << std::endl;
	return file_stringstream.str();

}

Wt::WContainerWidget* ChoiceQuestion::getWidget() const {
	Wt::WContainerWidget* ret (new Wt::WContainerWidget());
	Wt::WButtonGroup *group = new Wt::WButtonGroup(ret);
	group->setObjectName("answer");
	ret->addWidget(new Wt::WText(question_string));
	new Wt::WBreak(ret);
	for(int i = 0; i < amount_of_answers_; i++){
		group->addButton(new Wt::WRadioButton(answers_[i], ret), i);
		new Wt::WBreak(ret);
	}
	return ret;

}

