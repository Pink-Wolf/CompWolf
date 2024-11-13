/** complex_class description
 * @typeparam TemplateParameter TemplateParameter description
 */
template<template<typename> class TemplateParameter>
class complex_class
{
	/** private_member_should_be_ignored description
	 */
	void private_member_should_be_ignored()
	{

	}
public:
	// member_data description
	int member_data;

	/** enum_member description */
	enum class enum_member
	{
		/** enum1 description */
		enum1,
		/** this comment should be ignored */
		/** enum2 description */
		enum2,
		/** enum3 description */
		enum3
	};

	/** this comment should be ignored */
	/** alias_member description */
	using alias_member = int;

	/** member_macro description */
#define member_macro macro body

	/** nested_class description */
	class nested_class
	{
		/** nested_class_method private description */
		void nested_class_method()
		{

		}
	protected:
		/** nested_class_method protected description
		 * @param param1 param1 description
		 */
		void nested_class_method(int param1)
		{

		}
	public:
		/** nested_class_method public description */
		void nested_class_method(bool)
		{

		}
	};
};

/** ConceptName description */
template <typename T> concept ConceptName = requires(T a)
{
	{ a.test() };
};

/** simple_class description */
class simple_class
{

};

/** simple_struct description */
struct simple_struct
{

};

/** forward_declared_class description */
class forward_declared_class;

/** global_function description */
void global_function()
{

}
