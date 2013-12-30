/*
 * Group.cpp
 *
 *  Created on: 2-dec.-2013
 *      Author: arno
 */

#include "Group.h"
#include "QuestionList.h"
#include "Question.h"


Group::Group() {
	type_ = GROUP;

}

Group::Group(Path& id, std::string& theme_string, Question* question1,
		Question* question2) :
		Question(id, theme_string) {
	ql_.setCurrentPath(id_);
	question1->setOptional(optional_);
	question2->setOptional(optional_);
	ql_.add(question1);
	ql_.add(question2);
	type_ = GROUP;
}

Group::Group(Path& id, std::string& theme_string) :
		Question(id, theme_string) {
	ql_.setCurrentPath(id_);
	type_ = GROUP;
}

Path Group::add(Question* question) {
	return id_.cons(ql_.add(question));
}

void Group::ungroup(QuestionList& to_add) {
	ql_.copy_to_other_ql(to_add);
}

void Group::group_questions(Path q1, Path q2, std::string& theme_string) {
	ql_.group(q1, q2, theme_string);
}

std::string Group::get_question_file_string() const {
	std::stringstream ss;
	ss << id_.toString() << " " << Question::get_type_string(type_) << " "
			<< ql_.length() << " " << question_string << std::endl;
	ql_.save(ss);
	return ss.str();
}

std::string Group::get_string() const {
	return get_string(0);
}

Path Group::add(Question* question, Path& local_path) {
	question->setOptional(optional_);
	return id_.cons(ql_.add(question, local_path));
}

std::string Group::get_question_string(Path& local_path) const {
	return ql_.get_question_string(local_path);
}

void Group::delete_question(Path& local_path) {
	ql_.delete_question(local_path);
}

void Group::edit_choice(Path& local_path, std::string* answers, int amount) {
	ql_.edit_choice(local_path, answers, amount);
}

void Group::edit(Path& local_path, std::string& new_question_string) {
	ql_.edit(local_path, new_question_string);
}

Path Group::increase_id(int level) {
	ql_.increase_ids();
	return Question::increase_id(level);
}

Path Group::decrease_id(int level) {
	ql_.decrease_ids();
	return Question::decrease_id(level);
}

QuestionList::QLiterator * Group::getIterator() {
	return ql_.begin();
}

int Group::amountOfQuestions() const {
	return ql_.amountOfQuestions();
}

bool Group::empty() const {
	return ql_.amountOfQuestions() == 0;
}

int Group::amountOfQuestions(Path& p) const {
	return ql_.amountOfQuestions(p);
}

std::string Group::get_string(int level) const {
	std::stringstream ss;
	for (int i = 0; i < level; ++i) {
		ss << "     ";
	}
	ss << id_.toString() << " " << Question::get_type_string(type_) << " "
			<< amountOfQuestions() << " " << question_string << std::endl;
	ql_.list(&ss, level + 1);
	return ss.str();
}

void Group::ungroup_deep(Path& p) {
	ql_.ungroup(p);
}

Question* Group::getQuestion(Path& p) {
	return ql_.getQuestion(p);
}

Group::~Group() {
	//will call the destructor of the questionlist so that will handle the correct deletion
}

