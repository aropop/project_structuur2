/*
 * Parser.cpp
 *
 *  Created on: 14-nov.-2013
 *      Author: arno
 */
#include <iostream> //input output
#include <algorithm> //to lower case
#include <string> //::tolower
#include "Parser.h"
#include "QuestionList.h" //QuestionList
#include "Question.h" //QuestionTypes
#include "AnswerSet.h"
//variabelen initialiseren en parse loop starten
Parser::Parser(std::istream * in, std::ostream * out, QuestionList * ql) :
		in_(in), out_(out), ql_(ql), answer_fn_("") {
	parse_next();
}

Parser::Parser(std::istream* in, std::ostream* out, QuestionList* ql,
		const std::string& answer_fn) :
		in_(in), out_(out), ql_(ql), answer_fn_(answer_fn) {
	parser_code_ = TEST;
	current_it_ = *(ql_->begin());
	answers_ = AnswerSet(ql_);
	if (current_it_ != ql_->end()) {
		*out_ << (**current_it_).get_asking_string();
		getline(*in_, message_);
		parse_dispatch_tester(!answer_fn_.empty(), answer_fn_);
		parse_next();
	} else {
		*out_ << "Lege enquête!";
	}
}

Parser::~Parser() {

}

//parsed telkens voort
void Parser::parse_next() {
	while (parser_code_ != EXIT) { //stoppen bij een exit message
		if (parser_code_ == WRONG_MESSAGE) {
			*out_ << "Onbekend commando '" << message_ << "'" << std::endl;
		}
		if (parser_code_ == TEST) {
			getline(*in_, message_);
			parse_dispatch_tester(!answer_fn_.empty(), answer_fn_);
		} else {
			//resetten zodat enkel bij een "fout" er een set van de parser message moet gebeuren
			reset_parser_code();
			//inlezen en dispathen
			getline(*in_, message_);
			parse_dispatch_editor();
		}
	}
}

//dispatched de commando's
void Parser::parse_dispatch_editor() {
	std::stringstream ss;
	std::string command;

	ss.str(message_);
	ss >> command;

	//toelaten dat List en list hetzelfde zijn
	std::transform(command.begin(), command.end(), command.begin(), ::tolower);

	if (command.compare("list") == 0) {
		//list commando
		ql_->list(out_);
	} else if (command.compare("add") == 0) {
		//add commando
		std::string question_type, question;
		Path index(ql_->amountOfQuestions() + 1);
		getToNextChar(ss);
		if (ss.peek() >= '0' && ss.peek() <= '9') {
			ss >> index;
			try {
				index.push_number(ql_->amountOfQuestions(index) + 1); //achteraan toevoegen
			} catch (std::string& e) {
				*out_ << e << std::endl; //niet geldig pad
			}
		}
		Path print_path(index); //make a copy to be sure the reference functions don't edit it
		ss >> question_type;
		ss.ignore();
		getline(ss, question);
		if (question.empty()) {
			*out_ << "Incorrecte vraag tekst! (" << question << ")"
					<< std::endl;
		} else {
			//add is verschillend voor choice als voor tekst
			if (question_type.compare(
					Question::get_type_string(Question::CHOICE)) == 0) {
				//eerst nieuwe antwoorden prompten
				std::string * answers = prompt_for_choices();
				//nieuwe toevoegen geeft de index terug
				index = ql_->add(Question::CHOICE, question, answers,
						current_amount_of_answers_, index);
				print_add_text(question, print_path);
			} else if (question_type.compare(
					Question::get_type_string(Question::TEXT)) == 0) {

				//nieuwe toevoegen en commando afronden
				index = ql_->add(Question::TEXT, question, index);
				print_add_text(question, print_path);
			} else if (question_type.compare(
					Question::get_type_string(Question::BOOL)) == 0) {

				//nieuwe toevoegen en commando afronden
				index = ql_->add(Question::BOOL, question, index);
				print_add_text(question, print_path);
			} else if (question_type.compare(
					Question::get_type_string(Question::SCALE)) == 0) {
				//nieuwe toevoegen en commando afronden
				int min, max;
				*in_ >> min;
				*in_ >> max;
				in_->ignore();
				index = ql_->add(Question::SCALE, question, min, max, index);
				print_add_text(question, print_path);
			} else {
				//ongekend vraag type
				*out_ << "Niet gekend vraag type: " << question_type
						<< std::endl;
			}
		}
	} else if (command.compare("insert") == 0) {
		//insert commando
		std::string question_type, question;
		ss.ignore();
		Path position;
		ss >> position;
		//kijken of positie in range is
		if (ql_->in_range(position)) {
			ss >> question_type;
			ss.ignore();
			getline(ss, question);
			if (question.empty()) {
				*out_ << "Incorrecte vraag tekst! (" << question << ")"
						<< std::endl;
			} else {
				//bij choice moeten er nog antwoorden worden gevraagd
				if (question_type.compare(
						Question::get_type_string(Question::CHOICE)) == 0) {
					//choice antwoorden vragen en question toevoegen
					std::string * answers = prompt_for_choices();
					try {
						ql_->add(Question::CHOICE, question, answers,
								current_amount_of_answers_, position);
						print_add_text(question, position);
					} catch (std::string& e) {
						*out_ << e << std::endl;
					}
				} else if (question_type.compare(
						Question::get_type_string(Question::TEXT)) == 0) {
					//TEXT question bouwen en toevoegen
					try {
						ql_->add(Question::TEXT, question, position);
						print_add_text(question, position);
					} catch (std::string& e) {
						*out_ << e << std::endl;
					}
				} else if (question_type.compare(
						Question::get_type_string(Question::BOOL)) == 0) {
					try {
						//nieuwe toevoegen en commando afronden
						ql_->add(Question::BOOL, question, position);
						print_add_text(question, position);
					} catch (std::string& e) {
						*out_ << e << std::endl;
					}
				} else if (question_type.compare(
						Question::get_type_string(Question::SCALE)) == 0) {
					//nieuwe toevoegen en commando afronden
					int min, max;
					*in_ >> min;
					*in_ >> max;
					in_->ignore();
					try {
						ql_->add(Question::SCALE, question, min, max, position);
						print_add_text(question, position);
					} catch (std::string& e) {
						*out_ << e << std::endl;
					}
				} else {
					//onbekend type
					*out_ << "Niet gekend vraag type" << question_type
							<< std::endl;
				}
			}
		} else {
			//out of biound error printen
			print_out_of_bounds(position);
		}
	} else if (command.compare("edit") == 0) {
		//edit heeft 2 vormen
		ss.ignore();
		if ((ss.peek() >= '0') && (ss.peek() <= '9')) {
			//edit de vraagtekst
			Path index;
			ss >> index;
			//kijken binnen range
			if (ql_->in_range(index)) {
				std::string new_question_string =
						prompt_for_new_question_string(index);
				ql_->edit(index, new_question_string);
				*out_ << "Vraagtekst voor vraag " << index << " aangepast."
						<< std::endl;
			} else {
				//out of bounds error
				print_out_of_bounds(index);
			}
		} else {
			//edit de choices
			std::string ignore;
			Path index;
			ss >> ignore;
			ss >> index;
			if (ql_->in_range(index)) {
				//answers vragen en choices aanpassen
				*out_ << "Nieuwe antwoorden voor vraag " << index << " (";
				*out_ << ql_->get_question_string(index) << ")" << std::endl;
				std::string * answers = prompt_for_choices();
				//edit zou een error kunnen throwen wanneer dit een choice is
				try {
					ql_->edit_choice(index, answers,
							current_amount_of_answers_);
					*out_ << "Antwoorden voor vraag " << index << " aangepast."
							<< std::endl;
				} catch (std::string& e) {
					//de error gewoon printen
					*out_ << e << std::endl;
				}
			} else {
				//out of bounds error printen
				print_out_of_bounds(index);
			}
		}

	} else if (command.compare("remove") == 0) {
		//remove commando
		Path index;
		ss.ignore();
		ss >> index;
		//out of bounds
		if (ql_->in_range(index)) {
			//text vragen voor verwijderen
			//--index;
			std::string text(ql_->get_question_string(index));
			try {
				ql_->delete_question(index);
			} catch (std::string& e) {
				*out_ << e << std::endl;
			}
			*out_ << "Vraag " << index << " (" << text << ") verwijderd."
					<< std::endl;
		} else {
			//error weergeven
			print_out_of_bounds(index);
		}

	} else if (command.compare("group") == 0) {
		ss.ignore();
		Path p1;
		Path p2;
		ss >> p1;
		ss >> p2;
		if (p1.length() != p2.length()) {
			*out_ << "Paden zijn niet op het zelfde niveau (" << p1.toString()
					<< ", " << p2.toString() << std::endl;
		} else if (p1 == p2) {
			*out_ << "Geef 2 verschillende paden op het zelfde niveau!"
					<< std::endl;
		} else {
			std::string theme_string;
			getline(ss, theme_string);
			if (!theme_string.empty()) {
				try {
					if (p1 < p2) {
						ql_->group(p1, p2, theme_string);
					} else {
						ql_->group(p2, p1, theme_string);
					}
				} catch (std::string& e) {
					*out_ << e;
				}
			} else {
				*out_ << "Vul een geldige groepstekst in! (" << theme_string
						<< ")" << std::endl;
			}
		}
	} else if (command.compare("ungroup") == 0) {
		ss.ignore();
		Path p1;
		ss >> p1;
		try {
			ql_->ungroup(p1);
		} catch (std::string& e) {
			*out_ << e << std::endl;
		}
	} else if (command.compare("exit") == 0) {
		//exit commando
		//alleen vragen voor een save als er aanpassing zijn
		if (ql_->dirty) {
			if (prompt_save()) { //geeft bool terug
				ql_->save();
				*out_ << "Bestand bewaard." << std::endl;
			}
		}
		parser_code_ = EXIT; //parser_code op exit zetten om te stoppen
	} else if (command.compare("test") == 0) {
		parser_code_ = TEST; //parser_code op test zetten om in test modus om te schakelen
		current_it_ = *(ql_->begin());
		answers_ = AnswerSet(ql_);
		*out_ << (**current_it_).get_asking_string();
	} else if (command.compare("save") == 0) {
		//save commando
		if (ql_->dirty) {
			try {
				ql_->save();
				*out_ << "Bestand bewaard." << std::endl;
			} catch (std::string& e) {
				*out_ << e << std::endl;
			}
		} else {
			*out_ << "Bestand bewaard." << std::endl;
		}
	} else {
		//verkeerde message
		parser_code_ = WRONG_MESSAGE;
	}
}

//hulp functie die een pointer naar een stuk geheugen op de heap
//hierin zit een array met de hoeveelheid antwoorden
//current_amount_of_answers_ bevat dan het aantal antwoorden dat de gebruiker opgeeft
std::string* Parser::prompt_for_choices() {
	//variabelen initialiseren
	int array_size(5), amount_of_answers(0);
	std::string * answers = new std::string[array_size];
	std::string current_read;
	getline(*in_, current_read);
	//lopen tot de punt
	while (!(current_read.compare(".") == 0)) {
		amount_of_answers++;
		if (amount_of_answers > array_size) {
			//het stuk gealloceerde geheugen is niet groot genoeg
			//groter stuk aanvragen en opvullen
			array_size = array_size * 2;
			std::string * temp = answers;
			answers = new std::string[array_size];
			for (int shift = 0; shift < (array_size / 2); ++shift) {
				answers[shift] = temp[shift];
			}
			//oud stuk deleten
			delete[] temp;
		}
		//opvullen
		answers[amount_of_answers - 1] = current_read;
		getline(*in_, current_read);
	}
	current_amount_of_answers_ = amount_of_answers;
	//test of er genoeg antwoorden zijn
	if (amount_of_answers == 0) {
		*out_ << "Niet genoeg geldige antwoorden." << std::endl;
		answers = prompt_for_choices();
	}
	return answers;
}

//hulp functie die vraagt of een dirty lijst moet worden gesaved
bool Parser::prompt_save() {
	*out_
			<< "Er zijn onbewaarde wijzigingen. Moeten deze opgeslagen worden? (j/n)"
			<< std::endl;
	std::string userInput;
	getline(*in_, userInput);
	while ((userInput.compare("j") != 0) && (userInput.compare("n") != 0)) {
		*out_ << "Geef een juist antwoord (j/n)" << std::endl;
		getline(*in_, userInput);
	}
	return userInput.compare("j") == 0;
}

//hulp functie die de parser code reset
inline void Parser::reset_parser_code() {
	parser_code_ = CORRECT;
}

//hulp functie die de nieuwe vraagtekst terug geeft
std::string Parser::prompt_for_new_question_string(Path& index) {
	*out_ << "Nieuwe vraagtekst voor vraag " << index << " ("
			<< ql_->get_question_string(index) << ")" << std::endl;
	std::string new_question_string;
	getline(*in_, new_question_string);
	while (new_question_string.empty()) {
		*out_ << "Geef een niet lege vraag in!" << std::endl;
		getline(*in_, new_question_string);
	}
	return new_question_string;
}

//inline print functies om code reproductie te vermijden
void Parser::print_add_text(const std::string& question, Path position) const {
	*out_ << "Vraag (" << question << ") toegevoegd op plaats "
			<< position.toString() << "." << std::endl;
}

void Parser::print_out_of_bounds(Path index) const {
	*out_ << "Ongeldige invoer (" << index.toString() << "), N="
			<< ql_->amountOfQuestions() << std::endl;
}

void Parser::getToNextChar(std::stringstream& ss) {
	while (ss.peek() == ' ') {
		ss.ignore();
	}
}

void Parser::parse_dispatch_tester(bool save_answers,
		const std::string & file_name) {
	std::stringstream ss;
	std::string command;

	bool print_next_question(true);

	ss.str(message_);
	ss >> command;

	if (current_it_ == ql_->end()) //we eindigen op het einde
			{
		command = std::string(":quit");
		print_next_question = false;
	}

	if (special_test_command(command)) {
		command = command.substr(1, command.size());
		if (command.compare("quit") == 0) {
			if (answers_.fully_answered()) {
				print_next_question = false;
				if (save_answers) {
					parser_code_ = EXIT;
					answers_.write_to_file(file_name, ql_->getUuidString());
				} else {
					parser_code_ = CORRECT;
				}
			} else {
				*out_
						<< "De vragen lijst is niet volledig beantwoord, toch eindigen? (j/n)"
						<< std::endl;
				std::string j_n;
				getline(*in_, j_n);
				while ((j_n.compare("j") != 0) && (j_n.compare("n") != 0)) {
					*out_ << "j of n invullen!" << std::endl;
					getline(*in_, j_n);
				}
				if (j_n.compare("j") == 0) {
					parser_code_ = CORRECT;
					print_next_question = false;
					if (save_answers) {
						answers_.write_to_file(file_name, ql_->getUuidString());
					}
				}
			}
		} else if (command.compare("next") == 0) {
			int N(1);
			ss >> N;
			for (int i = 0; i < N; ++i) {
				++current_it_;
			}
		} else if (command.compare("previous") == 0) {
			int N(1);
			ss >> N;
			for (int i = 0; i < N; ++i) {
				--current_it_;
			}
		} else if (command.compare("goto") == 0) {
			Path goto_path;
			ss >> goto_path;
			try {
				Question* q(ql_->getQuestion(goto_path));
				if (q->getType() == Question::GROUP) {
					*out_ << "Geen geldige vraag om naar te springen"
							<< std::endl;
				}
				while (!(goto_path == current_it_.getPath())) {
					if (goto_path < current_it_.getPath()) {
						--current_it_;
					} else {
						++current_it_;
					}
				}
			} catch (std::string& e) {
				*out_ << e << std::endl;
			}
		} else if (command.compare("list") == 0) {
			answers_.list(*out_);
			print_next_question = false;
		} else {
			*out_ << "Niet gekend test commando '" << command << "'"
					<< std::endl;
		}
	} else {
		try {
			Path path(current_it_.getPath());
			Answer a(message_, path);
			answers_.add(a);
			++current_it_;
		} catch (std::string& e) {
			*out_ << e << std::endl;
		}
	}
	if ((current_it_ == ql_->end()) && save_answers
			&& answers_.fully_answered()) {
		parser_code_ = EXIT;
		answers_.write_to_file(file_name, ql_->getUuidString());
	} else if (current_it_ == ql_->end() && answers_.fully_answered()) {
		parser_code_ = CORRECT;
	} else if (current_it_ == ql_->end()) {
		*out_
				<< "Beantwoord eerst alle vragen voordat je de enquête kan beëindigen"
				<< std::endl;
	} else if (print_next_question && (current_it_ != ql_->end())) {
		*out_ << (**current_it_).get_asking_string();
	}

}

bool Parser::special_test_command(const std::string& command) const {
	return command.at(0) == ':';
}

