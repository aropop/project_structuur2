/*
 * WebApplication.h
 *
 *  Created on: 26-dec.-2013
 *      Author: arno
 */

#ifndef WEBAPPLICATION_H_
#define WEBAPPLICATION_H_

#include <Wt/WApplication>
#include "QuestionList.h"


class WebApplication: public Wt::WApplication {
public:
	WebApplication(const Wt::WEnvironment& env);
	~WebApplication();
private:

	enum Pages{
		IN_SURVEY, HOME
	};

	Pages mode_;
	QuestionList current_ql_;
	std::vector<std::string> surveys_done_;
	std::string survey_name_;

	void startSurvey(const std::string& filename);
	void submitSurvey();
	void showSurveys();
	void dispatch_pages();

};

#endif /* WEBAPPLICATION_H_ */
