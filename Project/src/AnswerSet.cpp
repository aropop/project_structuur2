/*
 * AnswerSet.cpp
 *
 *  Created on: 15-dec.-2013
 *      Author: arno
 */

#include "AnswerSet.h"
#include "QuestionList.h"
#include "Group.h"
#include <fstream>

AnswerSet::AnswerSet(QuestionList* ql) :
		ql_(ql) {

}
AnswerSet::AnswerSet() :
		ql_(NULL) {

}

AnswerSet::~AnswerSet() {

}

void AnswerSet::write_to_file(const std::string& file, const std::string& uuid) {
	std::fstream file_stream(file.c_str(), std::fstream::out);
	file_stream << "ID " << uuid << std::endl;
	int count(1);
	for (std::vector<Answer>::iterator it = vect_.begin(); it != vect_.end();
			it++) {
		file_stream << count++ << " " << (*it).getAnswer() << std::endl;
	}
	file_stream.close();
}

void AnswerSet::add(Answer& a) {
	std::string answer_str(a.getAnswer());
	Path copy(a.path);
	Question* question(ql_->getQuestion(copy));
	if (question->accepts_answer(answer_str)) {
		if (!vect_.empty()) {
			bool inserted(false);
			for (std::vector<Answer>::iterator it = vect_.begin();
					it != vect_.end(); it++) {
				if((*it).path == a.path){ //replacen
					it = vect_.erase(it);
					it = vect_.insert(it, a);
					inserted = true;
					return;
				}else if (!((*it).path < a.path)) {
					vect_.insert(it, a);
					inserted = true;
					return;
				}
			}
			if (!inserted) {
				vect_.push_back(a);
			}
		} else {
			vect_.push_back(a);
		}
	} else {
		throw std::string("Dit antwoord (").append(answer_str).append(
				") past niet op deze vraag");
	}
}

void AnswerSet::list(std::ostream& out) {
	std::vector<Answer>::iterator ans_it = vect_.begin();
	QuestionList::QLiterator q_it = *(ql_->begin());
	int cur_length(1); //if we go deeper we went by a group
	while (q_it != ql_->end()) {
		if (cur_length != q_it.getPath().length()) { //we gaan een groep binnen
			if (cur_length < q_it.getPath().length()) {
				Path p(q_it.getPath());
				int j;
				for (j = 0; j <= (p.length() - cur_length); ++j) {
					p.pop_number();
				}
				Path g(p);
				Question* quest(ql_->getQuestion(g));
				if (check_group_answered(quest, ans_it)) {
					out
							<< ql_->getQuestion(p)->get_ok_string(true,
									(q_it.getPath().length() - j)  - 1);
				} else {
					out
							<< ql_->getQuestion(p)->get_ok_string(false,
									(q_it.getPath().length() - j) - 1);
				}
				cur_length++;
			} else {
				cur_length--;
			}
		} else { //geen groep gewone vraag
			if (ans_it == vect_.end()) {
				out
						<< (*q_it)->get_ok_string(false,
								(*q_it)->getId().length() - 1);
			} else {
				if ((*ans_it).path == q_it.getPath()) {
					out
							<< (*q_it)->get_ok_string(true,
									(*q_it)->getId().length() - 1);
					ans_it++;
				} else {
					out
							<< (*q_it)->get_ok_string(false,
									(*q_it)->getId().length() - 1);
				}
			}
			++q_it;
		}
	}
}

bool AnswerSet::check_group_answered(Question* group,
		std::vector<Answer>::iterator a_it) {
	if (group->getType() != Question::GROUP) {
		throw std::string("Groep om te checken is geen groep").append(
				Question::get_type_string(group->getType()));
	}
	Group* casted_group(static_cast<Group*>(group));
	for (QuestionList::QLiterator q_it = *((*casted_group).getIterator());
			!q_it.ended(); ++q_it) {
		if (a_it == vect_.end()) {
			return false;
		} else {
			Path ans_path((*a_it).path);
			ans_path.pop_front_number();
			if (ans_path == q_it.getPath()) {
				a_it++;
			} else {
				return false;
			}
		}
	}
	return true;
}

bool AnswerSet::fully_answered() {
	std::vector<Answer>::iterator ans_it = vect_.begin();
	QuestionList::QLiterator q_it = *(ql_->begin());
	while(q_it != ql_->end()){
		if(ans_it == vect_.end() || !((*ans_it).path == q_it.getPath())){
			return false;
		}else{
			++q_it;
			++ans_it;
		}
	}
	return true;
}


