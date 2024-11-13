template<template<typename> class TemplateParameter>
class complex_class
{
	void private_member_should_be_ignored()
	{

	}
public:

	int member_data;

	enum class enum_member
	{
		enum1,
		enum2,
		enum3
	};

	using alias_member = int;

#define member_macro macro body

	class nested_class
	{
		void nested_class_method()
		{

		}
	protected:
		void nested_class_method(int param1)
		{

		}
	public:
		void nested_class_method(bool)
		{

		}
	};
};

template <typename T> concept ConceptName = requires(T a)
{
	{ a.test() };
};

class simple_class
{

};

struct simple_struct
{

};

class forward_declared_class;

void global_function()
{

}
