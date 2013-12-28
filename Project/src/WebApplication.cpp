/*
 * WebApplication.cpp
 *
 *  Created on: 26-dec.-2013
 *      Author: arno
 */

#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WContainerWidget>
#include <Wt/WEnvironment>
#include <Wt/WPushButton>
#include <Wt/WWidget>
#include <vector>
#include "dirent.h"
#include "WebApplication.h"
#include "QuestionList.h"
#include "Question.h"

#define _TITLE_ "Enquete"
#define _BR_() root()->addWidget(new Wt::WBreak())
#define _add_widget_(widget) root()->addWidget(widget)
#define _d_(m) std::cout << std::endl << "-------" << std::endl << m << std::endl << "-------" << std::endl;

WebApplication::WebApplication(const Wt::WEnvironment& env) :
		WApplication(env), mode_(HOME) {
	setTitle(_TITLE_);
	WApplication::instance()->internalPathChanged().connect(this,
			&WebApplication::dispatch_pages);
	dispatch_pages();
}

WebApplication::~WebApplication() {
	// TODO Auto-generated destructor stub
}

void WebApplication::startSurvey(const std::string& filename) {
	current_ql_ = QuestionList(std::string("src/").append(filename));
	_d_(filename);
	for(QuestionList::QLiterator it = *(current_ql_.begin()); it != current_ql_.end(); ++it){
		_add_widget_((**it).getWidget());
		_BR_();
	}
	Wt::WPushButton* submit (new Wt::WPushButton("Ingeven!", root()));
	submit->clicked().connect(this, &WebApplication::submitSurvey);
	_add_widget_(submit);
}

void WebApplication::showSurveys() {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("./src/")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string fname(ent->d_name);
			if (fname.length() > 3) {
				if (fname.substr(fname.length() - 3, fname.length()).compare(
						"ens") == 0) {
					std::string stitle(fname.substr(0, fname.length() - 4));
					Wt::WAnchor* anch(
							new Wt::WAnchor(
									Wt::WLink(Wt::WLink::InternalPath,
											"/survey/" + stitle),
									stitle.c_str(), root()));

					_add_widget_(anch);
					_BR_();
				}
			}
		}
		closedir(dir);
		WApplication::instance()->internalPathChanged().connect(this,
				&WebApplication::dispatch_pages);
	} else {
		/* could not open directory */
		root()->addWidget(new Wt::WText("Interne fout"));
	}
}

void WebApplication::submitSurvey() {
	AnswerSet answerset();
	std::vector<Wt::WWidget*> question_conts(root()->children());
	for(std::vector<Wt::WWidget*>::iterator it = question_conts.begin(); it != question_conts.end(); it++){
		Wt::WContainerWidget* wc(static_cast<Wt::WContainerWidget*>(*it));
		Wt::WObject obj (wc->find("answer"));
		Wt::WLineEdit* le(dynamic_cast<Wt::WLineEdit*>(obj));
		if(le != NULL){
			std::string answer(le->text());

		}else{

		}
	}
}

void WebApplication::dispatch_pages() {
	std::string path(internalPath());
	root()->clear();
	path = path.substr(1, path.length()); // de eerste '/' heeft geen nut
	if (path.empty()) {
		_add_widget_(new Wt::WText("Welkom bij de enqu&ecirc;te tool!"));
		_BR_();
		showSurveys();
	} else if (path.length() > 7) {
		if (path.substr(0, 6).compare("survey") == 0) {
			startSurvey(path.substr(7, path.length()).append(".ens"));
		}else{
			_add_widget_(new Wt::WText("Er is een fout opgetreden!0"));
		}
	} else {
		_add_widget_(new Wt::WText("Er is een fout opgetreden!"));
	}
}

