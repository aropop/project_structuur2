/*
 * AdminApplication.cpp
 *
 *  Created on: 30-dec.-2013
 *      Author: arno
 */

#include "AdminApplication.h"
#include "QuestionList.h"
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WDialog>
#include <Wt/WFileResource>
#include <Wt/WFileUpload>
#include <Wt/WLineEdit>
#include "dirent.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <stdio.h>

//#include <functional>

#define _TITLE_ "Admin"
#define _d_(m) std::cout << std::endl << "-------" << std::endl << m << std::endl << "-------" << std::endl;

AdminApplication::AdminApplication(const Wt::WEnvironment& env) :
		WApplication(env) {
	setTitle(_TITLE_);
	WApplication::instance()->internalPathChanged().connect(
			boost::bind(&AdminApplication::dispatch_pages, this));
	dispatch_pages();
}

void AdminApplication::dispatch_pages() {
	root()->clear();
	std::string path(internalPath());

	root()->addWidget(new Wt::WText("Welkom op het admin paneel!"));
	root()->addWidget(new Wt::WBreak());

	Wt::WTable* tab(new Wt::WTable(root()));

	tab->elementAt(0, 0)->addWidget(new Wt::WText("Naam"));
	tab->elementAt(0, 1)->addWidget(new Wt::WText("Ingevuld"));
	tab->setAttributeValue("style", "text-align:center");

	int counter(1);
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("./src/")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string fname(ent->d_name);
			if (fname.length() > 3) {
				if (fname.substr(fname.length() - 3, fname.length()).compare(
						"ens") == 0) {
					std::string stitle(fname.substr(0, fname.length() - 4));
					std::stringstream ss;
					ss << count_answers(stitle);
					tab->elementAt(counter, 0)->addWidget(
							new Wt::WText(stitle));
					tab->elementAt(counter, 1)->addWidget(
							new Wt::WText(ss.str()));
					Wt::WPushButton* remove_button(
							new Wt::WPushButton("Verwijderen",
									tab->elementAt(counter, 2)));
					remove_button->clicked().connect(
							boost::bind(&AdminApplication::show_dialog, this,
									stitle));
					Wt::WAnchor* answer_button(
							new Wt::WAnchor(json_answers(stitle),
									"Antwoorden"));
					tab->elementAt(counter, 3)->addWidget(answer_button);
					counter++;
				}
			}
		}
		closedir(dir);
	}
	root()->addWidget(new Wt::WBreak());
	Wt::WLineEdit* survey_name(new Wt::WLineEdit(root()));
	survey_name->setObjectName("survey_name");
	root()->addWidget(upload_button());

}

AdminApplication::~AdminApplication() {
	// TODO Auto-generated destructor stub
}

int AdminApplication::count_answers(const std::string& survey_title) {
	int count(0);
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("./src/")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string fname(ent->d_name);
			if (fname.length() > survey_title.length()) {
				if ((fname.substr(fname.length() - 3, fname.length()).compare(
						"ena") == 0)
						&& (fname.substr(0, survey_title.length()).compare(
								survey_title) == 0)) {
					count++;
				}
			}
		}
		closedir(dir);
	}
	return count;
}

void AdminApplication::show_dialog(const std::string& survey_title) {
	Wt::WDialog* dialog(new Wt::WDialog("Enquete verwijderen?"));
	dialog->contents()->addWidget(
			new Wt::WText(
					std::string("Ben je zeker dat je survey '").append(
							survey_title).append("' wilt verwijderen?")));
	dialog->contents()->addWidget(new Wt::WBreak());
	Wt::WPushButton* yes(new Wt::WPushButton("Ja"));
	Wt::WPushButton* no(new Wt::WPushButton("Nee"));

	yes->clicked().connect(
			boost::bind(&AdminApplication::remove_survey, this, survey_title,
					dialog));
	no->clicked().connect(
			boost::bind(&AdminApplication::remove_dialog, this, dialog));
	dialog->contents()->addWidget(yes);
	dialog->contents()->addWidget(no);

	dialog->accept();
	dialog->enable();
	dialog->exec();
}

void AdminApplication::remove_survey(const std::string& survey_title,
		Wt::WDialog* dia) {
	dia->disable();
	dia->contents()->clear();
	if (remove(
			std::string("./src/").append(survey_title).append(".ens").c_str())
			!= 0) {
		Wt::WText* error(
				new Wt::WText("Fout bij het verwijderen van het bestand!"));
		error->setAttributeValue("style", "color:red");
		dia->contents()->addWidget(error);
	} else {
		Wt::WText* succ(new Wt::WText("Succesvol verwijderd!"));
		succ->setAttributeValue("style", "color:green");
		dia->contents()->addWidget(succ);
	}
	dia->contents()->addWidget(new Wt::WBreak());
	Wt::WPushButton* close(new Wt::WPushButton("Sluiten"));
	close->clicked().connect(
			boost::bind(&AdminApplication::remove_dialog, this, dia));
	dia->contents()->addWidget(close);
	dia->enable();
}

Wt::WFileResource* AdminApplication::json_answers(
		const std::string& survey_title) {
	std::stringstream json;
	json << "[ ";
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("./src/")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string fname(ent->d_name);
			if (fname.length() > survey_title.length()) {
				if ((fname.substr(fname.length() - 3, fname.length()).compare(
						"ena") == 0)
						&& (fname.substr(0, survey_title.length()).compare(
								survey_title) == 0)) {
					json << "[";
					std::ifstream file(
							std::string("./src/").append(fname).c_str());
					std::string path, answer, line;
					std::stringstream ss;
					getline(file, line); //ignore first line
					getline(file, line);
					while (!file.eof()) {
						ss.clear();
						ss.str(line);
						ss >> path;
						ss >> line;
						bool is_int(false);
						int int_checker(0); //check for int if int there should be no '"' around it
						while ((unsigned) int_checker < line.length()) {
							is_int = isdigit(line[int_checker]);
							int_checker++;
						}
						if (is_int || line.compare("null") == 0) {
							json << line;
						} else {
							json << std::string("\"").append(line).append("\""); //operator << causes spacing
						}
						getline(file, line);
						if (!file.eof())
							json << ", ";
					}
					json << " ]";
				}
			}
		}
		closedir(dir);
	}
	json << "]";
	std::ofstream file(std::string(survey_title).append(".json").c_str());
	file << json.str();
	file.close();
	Wt::WFileResource* file_resource(
			new Wt::WFileResource("text/json",
					std::string(survey_title).append(".json").c_str()));
	file_resource->suggestFileName("answers.json");
	return file_resource;
}

void AdminApplication::remove_dialog(Wt::WDialog* dialog) {
	delete dialog;
	dispatch_pages();
}

Wt::WPushButton* AdminApplication::upload_button() {
	Wt::WFileUpload *upload = new Wt::WFileUpload(root());
	upload->setFileTextSize(10000);

	Wt::WPushButton *uploadButton = new Wt::WPushButton("Toevoegen", root());

	uploadButton->clicked().connect(upload, &Wt::WFileUpload::upload);
	uploadButton->clicked().connect(uploadButton, &Wt::WPushButton::disable);

	upload->changed().connect(upload, &Wt::WFileUpload::upload);
	upload->changed().connect(uploadButton, &Wt::WPushButton::disable);

	upload->uploaded().connect(
			boost::bind(&AdminApplication::fileUploaded, this, upload));

	upload->fileTooLarge().connect(this, &AdminApplication::tooLarge);
	return uploadButton;
}

void AdminApplication::fileUploaded(Wt::WFileUpload* fu) {
	std::string filename(fu->uploadedFiles().at(0).spoolFileName());
	try {
		std::string survey_name;
		QuestionList test(filename);
		Wt::WLineEdit* survey_line_edit(
				static_cast<Wt::WLineEdit*>(root()->find("survey_name")));
		survey_name = survey_line_edit->text().toUTF8();
		if (survey_name.empty()) {
			makeDialog("Geef een naam voor je enquête", "Sluiten");
		} else {
			test.save(std::string("src/").append(survey_name).append(".ens"));
			dispatch_pages();
		}
	} catch (std::string& e) {
		makeDialog("Niet geldig bestand", "Sluiten");
	}
}

void AdminApplication::tooLarge() {
	makeDialog("Bestand te groot", "Sluiten");
}

void AdminApplication::makeDialog(const std::string& text,
		const std::string& close) {
	Wt::WDialog* dialog(new Wt::WDialog(text));
	Wt::WPushButton* no(new Wt::WPushButton(close));
	no->clicked().connect(
			boost::bind(&AdminApplication::remove_dialog, this, dialog));
	dialog->contents()->addWidget(no);
	dialog->exec();
}

