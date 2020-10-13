# C++ Coding Style Guidelines

## Naming conventions

- Variables should be written in `snake_case`, for readability reasons: names are easier to read if words are separated with almost-blank caracters. Always prefers understandable names, even if they are longer !
```c++
	float circle_diameter = 2.0f * pi * circle_radius;
	long sense_of_life_and_universe_and_everything = 42;
```
	
- Macros and enumeration values should be written in `UPPER_SNAKE_CASE`. Constant variables should be written as normal `snake_case`.
```c++
	#define MAX_ITERATIONS 200
	constexpr float pi = 3.1415;
```
  
- Functions should be written in `snake_case`. 
```c++
	int try_read_buffer(char* buffer, std::size_t max_size);
```

- Namespace should be written `snake_case`. Unlike variables, they should be abbreviated when possible.
```c++
	namespace cfg { 
		// configuration namespace
	}; 
```
  
- Types should be written in `CamelCase`. This marks the difference between variables and types, between functions and class constructors,
and between classes and namespaces.
```c++
	class DatabaseProxy
	{};
	enum Direction
	{ LEFT, UP, RIGHT, DOWN };
	
	// this is a function call
	auto result = compute_something(a, b, c);
	// this is an object construction
	auto object = Something(a, b, c);
	
	// This is a static member function call
	Something::call_me_please();
	// This a standalone function call 
	cfg::call_me_please();
	
	// Type is a nested class 
	Something::Type my_var;
	// Type is a normal class in a namespace 
	cfg::Type my_var;
```

- An additional underscore should be placed before private members. It give an hint to quickly differentiate private and public members, and it avoids name conflicts.
```c++
	class List 
	{
	public:
		List(std::size_t size);
		
		/* ... */
		
	private:
		std::size_t _size;
	};
	
	List::List(std::size_t size)
	{
		// no conflicts. 
		// easy to know if a variable is a member or a parameter.
		_size = size;
	}
```
  
- Files and Folders names should be written in `snake_case`. Use C++ special extensions (`.hpp` instead of `.h`).
```
	src/
		config.hpp
		main.cpp
```
	
- Getters should be written without the useless `get_` prefix. It make names smaller without loss of understanding.
```c++
	class Person 
	{
		// ...
		
		std::string const& name() const;  
		// instead of 
		// std::string const& get_name() const;
		
		// ...
	};
	
	Person person;
	std::cout << person.name() << std::endl;
	// instead of 
	// std::cout << person.get_name() << std::endl;
```

## Style, line breaks and indentation

Use clang-format to automatically set the style of your code ! 

Tip: add it to your git pre-commit hook to avoid to forget it.

## Coding Guidelines 

- Classes headers should be considered as a part of the documentation. The reader should understand how to use a class by only reading the header and should have to look at the source file only if he want to study the class internal behavior. This implies:
	- Comments about how to use functions should be put in the header, using doxygen format.
	- Public members should be grouped and put at the beginning of the class declaration.
	- Inline functions should be put after the class declaration, using the `inline` keyword.
```c++
	class Something 
	{
		// public members at the beginning, with documentation
	public:
		Something();
		/** [doxygen doc] */
		bool do_the_task();
		/** [doxygen doc] */
		int internal_counter() const;
		
		// private members at the end (not useful to know how to use the class)
	private:
		int _internal_counter;
	};
	
	// inline functions after class declaration 
	//     (not useful to know how to use the class)
	inline Something::internal_counter() const 
	{
		return _internal_counter;
	}
	// other implementations in the source file
```
	
- Mark variables and functions as `const` whenever it is possible. Also use references and constant references every time you find it useful, especially to avoid copy of big objects.
```c++
	void do_something(std::vector<int> const& very_long_array);
	bool execute_algo(Input const& input, Output& output);
```

- Namespace names should be repeated before anything that come from this namespace, instead of using `using namespace xxx;` statements. It gives more information about the function / type definition, and helps to make difference between functions coming from libraries and functions created elsewhere in the project.
```c++
	// this comes from the standard library 
	std::variant<int, float> my_variant;
	// this comes from the boost library 
	boost::variant<int, float> my_variant;
	
	// we don't know, we need to search the code 
	// for the corresponding "using namespace xxx;".
	variant<int, float> my_variant;
```

- Functions implementations in source files should be done outside namespaces. 
```c++
	// Header file
	namespace ex {
		int sum(int a, int b, int c);
	};
	
	// Source file
	int ex::sum(int a, int b, int c)
	{
		return a + b + c;
	}
	// instead of 
	// namespace ex {
	// 	int sum(int a, int b, int c) /* ... */
	// };
```

- Casts should be done using special C++ keywords: `static_cast<type>(variable)` instead of `(type) variable`

- Declare all your variables at the beginning of your functions. It helps the reader if they are grouped.

- Move `#include` statements as much as possible in source files. It makes the compilation faster.


## Example Code

File `src/exampleclass.hpp`
```c++
#pragma once
#include <vector>

namepace test {

class ExampleClass 
{
public:
	ExampleClass();
	
	/**
	 * Set the epsilon attribute.
	 * \param new_value The new value of epsilon.
	 */
	void set_epsilon(float new_value);
	/**
	 * Access the epsilon attribute.
	 * \return The epsilon value.
	 */
	float epsilon() const;
	
	/**
	 * Create a sequence of numbers between 0 and epsilon attribute.
	 * \return The sequence as vector.
	 */
	std::vector<int> sequence() const;
	
	/**
	 * Print the epsilon attribute in standard output.
	 */
	void print() const;
	
private:
	float _epsilon;
};

inline ExampleClass::epsilon() const 
{
	return _epsilon;
}

}; // namespace test
```

File `src/exampleclass.hpp`
```c++
#include "exampleclass.hpp"
#include <iostream> // not needed in the header

// Comments about how to use the functions are in the header

test::ExampleClass::ExampleClass() : _epsilon(0.0f)
{}

void test::ExampleClass::set_epsilon(float new_value)
{
	_epsilon = new_value;
}

std::vector<int> test::ExampleClass::sequence() const
{
	if(_number < 0.0f)
		return std::vector<int>();

	// Create array
	std::vector<int> array;
	std::size_t count = static_cast<std::size_t>(_epsilon);
	array.resize(count);
	
	// Fill values
	for(std::size_t i = 0; i < count; ++i)
	{
		array[i] = i;
	}
	
	return array;
}

void test::ExampleClass::print() const 
{
	std::cout << "epsilon=" << _epsilon << std::endl;
}
```

File `src/main.cpp`
```c++
#include "exampleclass.hpp'
#include <iostream>

int main(int argc, char* argv[])
{
	test::ExampleClass example_test;
	
	// Setup example object
	example_test.set_epsilon(7.8f);
	example_test.print();
	
	// Print sequence
	auto sequence = example_test.sequence();
	for(auto it = sequence.begin(); it != sequence.end(); ++it)
	{
		std::cout << *it << ", ";
	}
	std::cout << std::endl;
	
	return 0;
}
```
