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
#include <Wt/WButtonGroup>
#include <Wt/WLineEdit>
#include <Wt/WRadioButton>
#include <Wt/WSlider>
#include <vector>
#include <iostream>
#include "dirent.h"
#include "WebApplication.h"
#include "QuestionList.h"
#include "Question.h"
#include "AnswerSet.h"

#define _TITLE_ "Enquete"
#define _BR_() root()->addWidget(new Wt::WBreak())
#define _add_widget_(widget) root()->addWidget(widget)
#define _d_(m) std::cout << std::endl << "-------" << std::endl << m << std::endl << "-------" << std::endl;

WebApplication::WebApplication(const Wt::WEnvironment& env) :
		WApplication(env), mode_(HOME) {
	setTitle(_TITLE_);
	WApplication::instance()->internalPathChanged().connect(boost::bind(&WebApplication::dispatch_pages, this));
	dispatch_pages();
}

WebApplication::~WebApplication() {
	// TODO Auto-generated destructor stub
}

void WebApplication::startSurvey(const std::string& filename) {
	std::string full_filename = std::string("src/").append(filename);
	current_ql_ = QuestionList(full_filename);
	for(QuestionList::QLiterator it = *(current_ql_.begin()); it != current_ql_.end(); ++it){
		Question* q (*it);
		_d_(q->getId().toString());
		Wt::WContainerWidget* w (q->getWidget());
		_add_widget_(w);
		_BR_();
	}
	Wt::WPushButton* submit (new Wt::WPushButton("Ingeven!", root()));
	submit->clicked().connect(boost::bind(&WebApplication::submitSurvey, this));
	_add_widget_(submit);
	survey_name_ = filename.substr(0, filename.length() - 4);
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
	AnswerSet answerset(&current_ql_);
	std::vector<Wt::WWidget*> question_conts(root()->children());
	//fill the answerset with the answers
	for(std::vector<Wt::WWidget*>::iterator it = question_conts.begin(); it != question_conts.end(); it++){
		Wt::WContainerWidget* wc(static_cast<Wt::WContainerWidget*>(*it));
		Wt::WWidget* obj (wc->find("answer"));
		Wt::WLineEdit* le(dynamic_cast<Wt::WLineEdit*>(obj));
		std::stringstream ss_path(static_cast<Wt::WText*>(wc->find("path"))->text().toUTF8());
		Path current_path;
		ss_path >> current_path;
		if(le != NULL){
			std::string answer_str = le->text().toUTF8();
			Answer answer_obj (answer_str, current_path);
			answerset.add(answer_obj);
		}else{
			Wt::WButtonGroup* bg (dynamic_cast<Wt::WButtonGroup*>(obj));
			if(bg != NULL){
				Wt::WRadioButton* rb (bg->checkedButton());
				std::string ans_str(rb->valueText().toUTF8());
				Answer a_o(ans_str, current_path);
				answerset.add(a_o);
			}else{
				Wt::WSlider* ws(dynamic_cast<Wt::WSlider*>(obj));
				if(ws != NULL){
					std::string ans_str(ws->valueText().toUTF8());
					Answer a_o(ans_str, current_path);
					answerset.add(a_o);
				}else{
					throw std::string("Error: unknown type of Wwidget");
				}
			}
		}
	}
	std::string session_id (sessionId());
	if(answerset.fully_answered()){
		answerset.write_to_file(survey_name_.append(session_id).append(".ena"), current_ql_.getUuidString());
		surveys_done_.push_back(survey_name_.append(".ens"));
	}else{
		Wt::WText* fill_in(new Wt::WText("Vul alle verplichte velden volledig in!"));
		fill_in->setAttributeValue("style", "color:red");
		root()->insertWidget(1, fill_in);
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

