#pragma once
#include <map>
#include <string>
#include <deque>
#include <stack>
#include "nonstd/recursive_wrapper.hpp"

class path_name_tag_tree
{
	struct node
	{
		typedef std::map<std::string, nonstd::recursive_wrapper<node>> map_t;
		map_t _nodes;
		size_t _tag;

		node();
		node(const node& krs);
	};
public:
	class iterator;
public:
	path_name_tag_tree(const char sep_char, const size_t invalid_tag);

	void reset();

	bool set(const char* pathname);
	bool set(size_t tag);
	bool get(size_t& tag);
	bool set(std::string pathname, size_t tag);
	bool get(std::string pathname, size_t& tag);

	iterator create_iterator(std::string pathname = "");
	void setup_iterator(iterator& it, std::string pathname = "");
private:

	const char* get_name(std::string& pathname, std::string::iterator& it, size_t& i);
	node* get_node_ptr(std::string& pathname);
	node& get_node_ref(std::string& pathname);
	node* get_node(std::string& pathname, bool& endpoint);
private:
	const size_t _invalid_tag;
	const char _sep_char;
	node _root_node;
};

class path_name_tag_tree::iterator
{
private:
	typedef std::pair<node::map_t::iterator, node*> step_t;

public:
	iterator();
	iterator(void* p);
	iterator(const iterator& krc);

	void setup(void* p);
	
	bool step_stack_push();
	bool step_stack_pop(bool replace = true);

	bool step();
	bool step_in();
	bool step_out();
	bool step_reset();
	bool steping();
	size_t steps();

	const bool is_leaf();
	const std::string& name();
	const size_t& tag();
private:
	bool do_step(node* n);
private:
	std::deque<step_t> _steps;
	std::stack<step_t> _stack;
};