/*
 * AdminApplication.h
 *
 *  Created on: 30-dec.-2013
 *      Author: arno
 */

#ifndef ADMINAPPLICATION_H_
#define ADMINAPPLICATION_H_

#include <Wt/WApplication>
#include <Wt/WDialog>
#include <Wt/WPushButton>

class AdminApplication: public Wt::WApplication {
public:
	AdminApplication(const Wt::WEnvironment& env);
	~AdminApplication();

private:
	void dispatch_pages();
	int count_answers(const std::string& survey_title);
	void remove_survey(const std::string& survey_title, Wt::WDialog* dia);
	Wt::WFileResource* json_answers(const std::string& survey_title);
	void show_dialog(const std::string& survey_title);
	void remove_dialog(Wt::WDialog* dialog);
	Wt::WPushButton* upload_button();
	void fileUploaded(Wt::WFileUpload* fu);
	void tooLarge();
};

#endif /* ADMINAPPLICATION_H_ */
