/*
 * path.h
 *
 *  Created on: 30-nov.-2013
 *      Author: arno
 */

#include <iostream>
#include <vector>

#ifndef PATH_H_
#define PATH_H_

class Path {
public:
	Path();
	Path(int initialNumber);

	std::string toString() const;

	void push_number(int num);
	int pop_number();
	int peek_number();
	int pop_front_number();
	int peek_front() const;
	void push_front_number(int num);

	bool operator==(const Path& path2) const;
	Path operator++();
	Path operator--();

	bool at_end() const;
	bool empty() const;

	void clear();

	int length() const;

	Path cons(Path to_cons);

	friend bool operator< (const Path& other, const Path& p);
private:
	std::vector<int> numbers_;

};

std::ostream& operator << (std::ostream& out, Path p);
std::istream& operator>>(std::istream& in, Path &p);



#endif /* PATH_H_ */
