/*
 * Group.h
 *
 *  Created on: 2-dec.-2013
 *      Author: arno
 */

#ifndef GROUP_H_
#define GROUP_H_

#include "Question.h"
#include "QuestionList.h"

class Group: public Question {
public:
	Group();
	Group(Path& id, std::string& theme_string);
	Group(Path& id, std::string& theme_string, Question * question1, Question * question2);

	Path add(Question * question);
	Path add(Question * question, Path& local_path);

	void delete_question(Path& local_path);

	void edit_choice(Path& local_path, std::string* answers, int amount);
	void edit(Path& local_path, std::string& new_question_string);

	void group_questions(Path q1, Path q2, std::string& theme_string);
	void ungroup(QuestionList& to_add);
	void ungroup_deep(Path& p);

	virtual Path increase_id(int level = 0);
	virtual Path decrease_id(int level = 0);

	bool empty() const;

	int amountOfQuestions() const;
	int amountOfQuestions(Path& p) const;

	QuestionList::QLiterator * getIterator();

	Question* getQuestion(Path& p);

	std::string get_question_file_string() const;
	std::string get_string() const;
	std::string get_string(int level) const;
	std::string get_question_string(Path& local_path) const;
	~Group();

private:
	QuestionList ql_;
};

#endif /* GROUP_H_ */
