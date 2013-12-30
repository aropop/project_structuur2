/*
 * QuestionList.cpp
 *
 *  Created on: 10-nov.-2013
 *      Author: arno
 */

#include "QuestionList.h"
#include "Question.h"
#include "ChoiceQuestion.h"
#include "ScaleQuestion.h"
#include "BoolQuestion.h"
#include "Group.h"
#include <iostream>
#include <fstream>
#include <uuid/uuid.h>

QuestionList::QuestionList(const std::string& filename) :
		filename_(filename) {
	//ifstream opbouwen
	std::ifstream input_file(filename_.c_str());
	dirty = false;
	//verschil tussen een bestaand bestand en een niet bestaand bestand
	if (!input_file) {
		//bestand bestaat niet
		uuid_generate(uuid_);
	} else {
		//probeer te lezen van bestand
		try {
			read_from_file(&input_file);
		} catch (std::string& e) {
			//fout in bestand
			std::cout << e << std::endl;
		}
		//file sluiten
		input_file.close();
	}
}

//mostly used for groups
QuestionList::QuestionList() :
		dirty(false) {
}

QuestionList::~QuestionList(void) {
	//verwijder de questions
	for (std::vector<Question *>::iterator i = questions_.begin();
			i != questions_.end(); i++) {
		delete *i;
	}
}

//list commando
void QuestionList::list(std::ostream * out, int level) const {
	//over vragen lopen en ze uitprinten
	for (std::vector<Question*>::const_iterator i = questions_.begin();
			i != questions_.end(); ++i) {
		*out << (**i).get_string(level);
	}
}

//add commando met een positie
Path QuestionList::add(Question::QuestionType type,
		std::string& question_string, std::string *answers,
		int amount_of_answers, Path position) {
	Question * q;
	if (type == Question::TEXT) {
		q = new Question(current_path_.cons(position), question_string);
	} else if (type == Question::CHOICE) {
		q = new ChoiceQuestion(current_path_.cons(position), question_string,
				answers, amount_of_answers);
	} else if (type == Question::BOOL) {
		q = new BoolQuestion(current_path_.cons(position), question_string);
	} else {
		throw std::string(
				"You have to specify more for these type of questions");
	}
	return add(q, position);
}

//overloaden om add zonder positie mogelijk te maken
Path QuestionList::add(Question::QuestionType type,
		std::string& question_string, std::string *answers,
		int amount_of_answers) {
	return add(type, question_string, answers, amount_of_answers,
			amountOfQuestions() + 1);
}

//hulp functie die uuid omzet
std::string uuid_to_string(char * character_array) {
	std::string uuid_string;
	uuid_string.append(character_array, 36);
	return uuid_string;
}

//save commando
void QuestionList::save() {
	//output file openen
	std::fstream output_file(filename_.c_str(), std::ofstream::out);
	char uuid_str[36];
	uuid_unparse(uuid_, uuid_str);
	//header wegschrijven
	output_file << "VERSION 2" << std::endl << "ID " << uuid_to_string(uuid_str)
			<< std::endl << "STEPS " << deepAmountOfQuestions() << std::endl;
	save(output_file);
	//ql is niet meer dirty want is juist weggeschreven
	//moet alleen op toplevel gebeuren
	dirty = false;
}

void QuestionList::save(std::ostream& out) const {
	if (amountOfQuestions() == 0) {
		throw std::string("Lege groep kan niet opslaan!");
	} else {
		//vragen wegschrijven
		for (std::vector<Question *>::const_iterator i = questions_.begin();
				i != questions_.end(); i++) {
			out << (**i).get_question_file_string();
		}
	}
}

//hulp functie die zegt of een bepaalde positie binnen het bereik ligt
bool QuestionList::in_range(Path& position) const {
	return ((0 < position.peek_front())
			&& (position.peek_front() <= (amountOfQuestions() + 1)));
}

//geeft de question string door
std::string QuestionList::get_question_string(Path& index) const {
	if (index.length() > 1) {
		Path copy(index);
		return dynamic_cast<Group*>(questions_.at(copy.pop_front_number() - 1))->get_question_string(
				copy);
	} else {
		return questions_.at(index.peek_number() - 1)->question_string;
	}
}

//edit commando voor question strings
void QuestionList::edit(Path& question_number,
		std::string& new_question_string) {
	if (question_number.length() > 1) {
		dynamic_cast<Group*>(questions_.at(
				question_number.pop_front_number() - 1))->edit(question_number,
				new_question_string);
	} else {
		questions_.at(question_number.peek_number() - 1)->set_question_string(
				new_question_string);
		dirty = true;
	}
}

//edit commando voor choices
void QuestionList::edit_choice(Path& question_number, std::string* new_answers,
		int amount) {
	if (question_number.length() > 1) {
		Path copy(question_number);
		dynamic_cast<Group*>(questions_.at(copy.pop_front_number() - 1))->edit_choice(
				copy, new_answers, amount);
	} else {
		questions_.at(question_number.peek_number() - 1)->set_answers(
				new_answers, amount);
		dirty = true;
	}
}

//getter voor amount of questions
int QuestionList::amountOfQuestions() const {
	return questions_.size();
}

//delete commando
void QuestionList::delete_question(Path& question_number) {
	if (question_number.length() > 1) {
		Group* grp = dynamic_cast<Group*>(questions_.at(
				question_number.pop_front_number() - 1));
		if (grp != NULL) {
			grp->delete_question(question_number);
		} else {
			throw std::string(
					"Kan de vraag niet verwijderen, het opgegeven pad is geen groep!");
		}
	} else {
		//avoid memory leaks
		delete questions_.at(question_number.peek_number() - 1);
		questions_.erase(
				questions_.begin() + question_number.peek_number() - 1);
		for (std::vector<Question*>::iterator it = questions_.begin()
				+ question_number.peek_front() - 1; it != questions_.end();
				it++) {
			(**it).decrease_id(current_path_.length());
		}
	}
}

//functie die het bestand uitleest en het object invult
void QuestionList::read_from_file(std::ifstream * input_file) {
	std::stringstream ss;
	std::string current_line, current_identifier;
	getline(*input_file, current_line);
	//bekijk de versie
	if (current_line.compare("VERSION 2") == 0
			|| current_line.compare("VERSION 1") == 0
			|| current_line.compare("VERSION 3") == 0) {
		getline(*input_file, current_line);
		ss.clear();
		ss.str(current_line);
		ss >> current_identifier;
		//bekijk het id
		if (current_identifier.compare("ID") == 0) {
			//uuid van tekst naar uuid_t omzetten
			char uuid_txt[36];
			ss >> uuid_txt;
			uuid_parse(uuid_txt, uuid_);

			//size van de lijst uitlezen

			getline(*input_file, current_line);
			ss.clear();
			ss.str(current_line);
			ss >> current_identifier;
			int total_questions(0);
			ss >> total_questions;

			int counter(1);
			//deze dan verder invullen
			while (total_questions >= counter) {

				getline(*input_file, current_line);
				ss.clear();
				ss.str(current_line);

				Path question_number;
				std::string question_string, question_type;

				ss >> question_number;
				ss >> question_type;

				//verschil tussen choice en text questions
				if (question_type.compare(
						Question::get_type_string(Question::CHOICE)) == 0) {

					int amount_of_answers;
					ss >> amount_of_answers;
					ss.ignore();
					getline(ss, question_string);
					std::string* answers = new std::string[amount_of_answers];
					ss.clear();

					//lees de choices uit
					for (int i = 0; i < amount_of_answers; ++i) {
						getline(*input_file, answers[i]);
					}
					//maak de vraag aan
					ChoiceQuestion * current_question = new ChoiceQuestion(
							question_number, question_string, answers,
							amount_of_answers);
					add(current_question, question_number);
					//scope zal de current_question destructen maar we hebben ze gekopieerd naar het geheugen
					current_question->copied = true;
				} else if (question_type.compare(
						Question::get_type_string(Question::TEXT)) == 0) {
					ss.ignore();
					getline(ss, question_string);
					//moet alleen vraag uitlezen en question object aanmaken
					Question* current_question = new Question(question_number,
							question_string);

					add(current_question, question_number);

					ss.clear();

				} else if (question_type.compare(
						Question::get_type_string(Question::BOOL)) == 0) {
					ss.ignore();
					getline(ss, question_string);
					Question* current_question = new BoolQuestion(
							question_number, question_string);
					add(current_question, question_number);
					ss.clear();

				} else if (question_type.compare(
						Question::get_type_string(Question::SCALE)) == 0) {
					int min, max;
					ss >> min;
					ss >> max;
					ss.ignore();
					getline(ss, question_string);
					Question* current_question = new ScaleQuestion(
							question_number, question_string, min, max);
					add(current_question, question_number);
					ss.clear();
				} else if (question_type.compare(
						Question::get_type_string(Question::GROUP)) == 0) {
					int amount;
					ss >> amount;
					ss.ignore();
					getline(ss, question_string);
					Question * current_question = new Group(question_number,
							question_string);
					add(current_question, question_number);
					ss.clear();
					counter--; //moet niet opgehoogd worden wanneer je een group leest
				} else {
					//kent het question type niet
					throw std::string("Unknown question type");
				}
				counter++;

			}

		} else {
			throw std::string("Corrupt file");
		}
	} else {
		throw std::string("Corrupt file");
	}

}

Path QuestionList::add(Question::QuestionType type,
		std::string& question_string, int min, int max) {
	int p = amountOfQuestions() + 1;
	return add(type, question_string, min, max, p);

}

Path QuestionList::add(Question::QuestionType type,
		std::string& question_string, int min, int max, Path position) {
	return add(
			new ScaleQuestion(current_path_.cons(position), question_string,
					min, max), position);
}

Path QuestionList::add(Question* question) {
	return add(question, amountOfQuestions() + 1);
}

Path QuestionList::add(Question* question, Path position) {
	if (!in_range(position)) {
		throw std::string(
				"Vraag kan niet worden toegevoegd niet binnen bereik");
	}
	if (position.length() > 1) {
		if (position.peek_front() == amountOfQuestions()) {
			int pos_on_this_level(position.pop_front_number() - 1);
			Group* grp(dynamic_cast<Group*>(questions_.at(pos_on_this_level)));
			if (grp != NULL) { //test of het wel een groep is
				grp->add(question, position);
				//push to get right number to print
				position.push_front_number(pos_on_this_level);
			} else {
				throw std::string(
						"De positie waarop je wilt toevoegen is geen groep!");
			}
		} else {
			throw std::string(
					"Vraag ligt niet binnen het bereik van deze enquête!");
		}
	} else {
		questions_.insert(questions_.begin() + position.peek_front() - 1,
				question);
		int num(position.peek_front());
		question->setId(current_path_.cons(Path(num)));
		if (question->getType() != Question::GROUP) {
			for (std::vector<Question*>::iterator it = questions_.begin()
					+ position.peek_front(); it != questions_.end(); it++) {
				(**it).increase_id(current_path_.length());
			}
		} else {
			for (std::vector<Question*>::iterator it = questions_.begin()
					+ position.peek_front(); it != questions_.end(); it++) {
				//group neemt altijd 2 vragen dus de vragen erna moeten met 1 verlaagt worden
				(**it).decrease_id(current_path_.length());
			}
		}
	}
	dirty = true;
	return position;
}

Path QuestionList::add(Question::QuestionType type,
		std::string& question_string) {
	return add(type, question_string, amountOfQuestions() + 1);
}

Path QuestionList::add(Question::QuestionType type,
		std::string& question_string, Path position) {
	Path p(0);
	if (type == Question::TEXT) {
		p = add(new Question(current_path_.cons(position), question_string),
				position);
	} else if (type == Question::BOOL) {
		p = add(new BoolQuestion(current_path_.cons(position), question_string),
				position);
	} else {
		throw std::string("You have to specify more for this type of question");
	}
	return p;
}

Path QuestionList::getCurrentPath() const {
	return current_path_;
}

void QuestionList::setCurrentPath(Path currentPath) {
	current_path_ = currentPath;
}

void QuestionList::group(Path& question1, Path& question2,
		std::string& theme_string) {
	if (!in_range(question1) || !in_range(question2)) {
		throw std::string("Niet binnen bereik!\n");
	}
	if (question1.length() > 1) {
		//pass through to lower level
		int group(question1.pop_front_number());
		question2.pop_front_number();
		Group* grp(dynamic_cast<Group*>(questions_.at(group - 1)));
		if (grp != NULL) {
			grp->group_questions(question1, question2, theme_string);
		} else {
			throw std::string("Gegeven pad is klopt niet");
		}
	} else {
		//do it on this level
		//copy to bypass side effect
		int begin_i(question1.peek_front());
		int end(question2.peek_front());
		Group * grp;
		Path group_path(question1);
		Path r(current_path_.cons(group_path));
		if ((unsigned) question2.peek_number() > questions_.size()) {
			throw std::string(
					"Je probeert te groupen op een vraag niet binnen het bereik!");
		} else {
			if (begin_i == (end - 1)) {
				grp = new Group(r, theme_string,
						questions_.at(question1.peek_number() - 1),
						questions_.at(question2.peek_number() - 1));

				questions_.erase(
						questions_.begin() + question1.peek_number() - 1);
				questions_.erase(
						questions_.begin() + question2.peek_number() - 2);
			} else {
				grp = new Group(r, theme_string, questions_.at(begin_i - 1),
						questions_.at(begin_i));
				questions_.erase(questions_.begin() + begin_i - 1);
				questions_.erase(questions_.begin() + begin_i - 1);
				int already_added(0);
				for (int i = begin_i + 1; i < end; ++i) {
					grp->add(questions_.at((i - 2) - already_added));
					questions_.erase(questions_.begin() + begin_i - 1);
					already_added++;
				}
			}
			add(grp, begin_i);
		}
	}
}

int QuestionList::length() const {
	return questions_.size();
}

QuestionList::QLiterator::QLiterator(QuestionList * ql) :
		pos_(Path(1)), ql_(ql), deep_(false), ended_(false) {
	cur_it_ = ql_->questions_.begin();
	if (cur_it_ != ql_->questions_.end()) {
		if ((*cur_it_)->getType() == Question::GROUP) {
			deep_ = true;
			deep_iterator_ = dynamic_cast<Group*>(*cur_it_)->getIterator();
		}
	} else {
		ended_ = true;
		cur_it_ = ql_->questions_.end();
	}
}
QuestionList::QLiterator::QLiterator(QuestionList * ql, bool getEnd) :
		pos_(Path(1)), ql_(ql), deep_(false), ended_(false) {
	if (getEnd) {
		cur_it_ = ql_->questions_.end();
	} else {
		cur_it_ = ql_->questions_.begin();
		if (cur_it_ != ql_->questions_.end()) {
			if ((*cur_it_)->getType() == Question::GROUP) {
				deep_ = true;
				deep_iterator_ = dynamic_cast<Group*>(*cur_it_)->getIterator();
			}
		} else {
			cur_it_ = ql_->questions_.end();
			ended_ = true;
		}
	}
}

Question* QuestionList::QLiterator::operator *() {
	if (deep_) {
		return **deep_iterator_;
	} else {
		return *cur_it_;
	}
}

QuestionList::QLiterator& QuestionList::QLiterator::operator ++() {
	if (!deep_) {
		//we can do this on this level
		++cur_it_;
		++pos_;
		if (*this == ql_->end()) {
			ended_ = true; //let upper level know we are done here
		} else if ((*cur_it_)->getType() == Question::GROUP) {
			//jump deeper
			deep_iterator_ = dynamic_cast<Group*>(*cur_it_)->getIterator();
			deep_ = true;
		}
	} else {
		//pass the message to a level deeper
		++(*deep_iterator_);
		if (deep_iterator_->ended()) {
			//level is done so we jump back up
			deep_ = false;
			operator ++();
		}
	}
	return *this;
}

QuestionList::QLiterator QuestionList::QLiterator::operator --() {
	if (!deep_) {
		//we can do this on this level
		--cur_it_;
		--pos_;
		if (*this == ql_->end() || pos_.peek_number() < 1) {
			ended_ = true; //let upper level know we are done here
		} else if ((*cur_it_)->getType() == Question::GROUP) {
			//jump deeper
			deep_iterator_ = static_cast<Group*>(*cur_it_)->getIterator();
			deep_ = true;
		}
	} else {
		//pass the message to a level deeper
		--(*deep_iterator_);
		if (deep_iterator_->ended()) {
			//level is done so we jump back up
			deep_ = false;
			operator --();
		}
	}
	return *this;

}

bool QuestionList::QLiterator::operator ==(const QLiterator& it) const {
	if (deep_) {
		return (*deep_iterator_) == it;
	} else {
		return cur_it_ == it.cur_it_;
	}
}

QuestionList::QLiterator * QuestionList::begin() {
	return new QLiterator(this);
}

QuestionList::QLiterator QuestionList::end() {
	return QLiterator(this, true);
}

bool QuestionList::QLiterator::operator !=(const QLiterator& it1) {
	if (deep_) {
		return (*deep_iterator_) != it1;
	} else {
		return cur_it_ != it1.cur_it_;
	}
}

int QuestionList::deepAmountOfQuestions() {
	int i(0);
	for (QLiterator it = *(begin()); it != end(); ++it) {
		i++;
	}
	return i;
}

Path QuestionList::QLiterator::getPath() {
	if (deep_) {
		Path pad(deep_iterator_->getPath());
		return pos_.cons(pad);
	} else {
		return pos_;
	}
}
QuestionList::QLiterator::QLiterator() {
}

int QuestionList::amountOfQuestions(Path p) const {
	if (p.length() > 0) {
		Group* grp(
				dynamic_cast<Group*>(questions_.at(p.pop_front_number() - 1)));
		if (grp != NULL) {
			return grp->amountOfQuestions(p);
		} else {

			throw std::string("Geen geldig pad opgegeven!");
		}
	} else {
		return amountOfQuestions();
	}
}

void QuestionList::decrease_ids() {
	//moet hier met en vector omdat je anders de id's van de groepen niet mee updatet
	for (std::vector<Question*>::iterator it = questions_.begin();
			it != questions_.end(); it++) {
		(**it).decrease_id();
	}

}

void QuestionList::increase_ids() {
	//moet hier met en vector omdat je anders de id's van de groepen niet mee updatet
	for (std::vector<Question*>::iterator it = questions_.begin();
			it != questions_.end(); it++) {
		(**it).increase_id();
	}

}

void QuestionList::copy_to_other_ql(QuestionList& to_add) {
	Path path_in_above(current_path_.peek_number());
	to_add.delete_pointer(path_in_above);
	for (std::vector<Question*>::reverse_iterator it = questions_.rbegin();
			it != questions_.rend(); it++) {
		to_add.add(*it, Path(current_path_.peek_number()));
	}
	questions_.clear();
}

void QuestionList::ungroup(Path& p) {
	Question * q(questions_.at(p.peek_front() - 1));
	if (q->getType() == Question::GROUP) {
		Group* g(static_cast<Group*>(q));
		if (p.length() > 1) {
			p.pop_front_number();
			g->ungroup_deep(p);
		} else {
			g->ungroup(*this);
		}
	} else {
		throw std::string("Het opgegeven pad is geen groep");
	}
}

void QuestionList::delete_pointer(Path& path) {
	//does the same as delete_question without deleting the question object from the heap
	if (path.length() > 1) {
		Group* grp = dynamic_cast<Group*>(questions_.at(path.peek_front()));
		if (grp != NULL) {
			grp->delete_question(path);
		} else {
			throw std::string(
					"Kan de vraag niet verwijderen, het opgegeven pad is geen groep!");
		}
	} else {
		questions_.erase(questions_.begin() + path.peek_number() - 1);
		for (std::vector<Question*>::iterator it = questions_.begin()
				+ path.peek_front(); it != questions_.end(); it++) {
			(**it).decrease_id(current_path_.length());
		}
	}
}

Question* QuestionList::getQuestion(Path& p) {
	if (p.length() > 1) {
		Question* q(questions_.at(p.peek_front() - 1));
		if (q->getType() == Question::GROUP) {
			Group * g(static_cast<Group*>(q));
			p.pop_front_number();
			return g->getQuestion(p);
		} else {
			throw std::string("Het opgegeven pad is geen vraag");
		}
	} else {
		if ((unsigned) p.peek_front() > questions_.size()) {
			throw std::string(
					"Pad ligt niet binnen het bereik van deze enquête");
		}
		return questions_.at(p.peek_front() - 1);
	}
}

std::string QuestionList::getUuidString() {
	char c_uuid[36];
	uuid_unparse(uuid_, c_uuid);
	return uuid_to_string(c_uuid);
}

