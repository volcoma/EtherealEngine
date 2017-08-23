#ifndef args_hpp_included
#define args_hpp_included
//
//  **args** is a fast, small, simple, powerful, single-header library for parsing command line arguments in C++
//  using more-or-less POSIX parsing rules.
//
//  To use:
//
//  	1.	Include the header:
//
//  			#include "args.hpp"
//
//  		There's no .lib or .so or .cpp or anything else to muck around with. It's all here.
//
//  	2.	Create a parser object and give it your program name and description (for the "usage" output):
//
//  			args::parser parser( "<my-prog>", "<description>" );
//
//  	3.	Add your arguments, templated on the desired type. Example:
//
//  			const auto& myArgument = parser.add< float >( ... );
//
//  	4.	In the add() function you'll indicate the long-form ("--example") and short-form ("-e") switches for this 
//  		argument. You can nullify one or the other if you want.
//  		You should specify a description (the second argument).
//  		You may also specify whether the argument is optional or required, and what the default value is (the value
//  		to be used if the user doesn't supply it):
//
//  			... parser.add< float >(
//  					"<long-form>",			// Use "example" if you want the user to say "--example", 
//  											// or leave blank for no long-form.
//  					"<description>",		// For "usage" output.
//  					'<short-form>', 		// A character. Use '\0' for no short-form.
//  					[args::optional|args::required], // To indicate whether it's required.
//  					[default-value] 		// The default value. Defaults to 0 or its equivalent for this type.
//  				);
//
//  	5.	Let 'er rip:
//
//  			parser.parse( argc, argv );
//
//  		Use try...catch... if you want to catch problems in a healthy way.
//
//  	6.	Access argument values:
//
//  			myArgument.value()		// Returns a float if we templated on <float>.
//
//  		try...catch... helps here too because users may give invalid strings for non-string types.
//
//  	7.	Call parser.show_usage() if you want to print help text.
//
//  	8.	Do other stuff. See "Example usage" below for "unlabeled" arguments, the "--" marker, and such.
//
//  Example usage:
//
//  ------------------------------------------------------------------------------------------------------------------
//
//  #include "args.hpp"
//
//  int main( int argc, const char* argv[] )
//  {
//  	args::parser parser( "argssample", "Parses a mix of many argument types and combinations." );
//  	
//  	const auto& useAscii = parser.add< bool >( "ascii",
//  											  "Use ASCII instead of that UNICORN thing or whatever it is.",
//  											  'a',
//  											  args::optional,
//  											  true /* defaults to true */ );
//  	const auto& runFast = parser.add< bool >( "fast", "Should this program run fast?", 'f' );
//  	const auto& doCalibrate = parser.add< bool >( "calibrate", "Calibrate sensors." /* no short-form */ );
//  	const auto& kindaOdd = parser.add< bool >( "", "No long-form argument for this one.", 'o' );
//  	const auto& mass = parser.add< float >( "mass", "The mass of the thing.", 'm', args::optional, 3.141f );
//  	const auto& mana = parser.add< float >( "mana", "The mana of the thing.", 'M' );
//  	const auto& height = parser.add< int >( "height", "The height of the thing.", 'h', args::required );
//  	const auto& name = parser.add< std::string >( "name", "The name of the wind.", 's', args::required );
//  	
//  	// Unleash the hounds.
//  	//
//  	try
//  	{
//  		// Example command line:
//  		//	argssample --height=16.25 -fo unlabeled --name="Absolom, Absolom" -h=8 -- --weirdly-unlabeled
//  		parser.parse( argc, argv );
//  	}
//  	catch( const std::exception& e )
//  	{
//  		std::cerr << "Error parsing arguments: " << e.what() << std::endl;
//  		parser.show_usage( std::cerr );
//  		exit( 1 );
//  	}
//
//  	// Get argument values.
//  	//
//  	try
//  	{
//  		std::cout << std::boolalpha;	// So that bool values come through as "true/false" rather than "1/0".
//  		std::cout << "useAscii=" << useAscii.value() << std::endl;
//  		std::cout << "runFast=" << runFast.value() << std::endl;
//  		std::cout << "doCalibrate=" << doCalibrate.value() << std::endl;
//  		std::cout << "kindaOdd=" << kindaOdd.value() << std::endl;
//  		std::cout << "mass=" << mass.value() << std::endl;
//  		std::cout << "mana=" << mana.value() << std::endl;
//  		std::cout << "height=" << height.value() << std::endl;
//  		std::cout << "name=" << name.value() << std::endl;
//  		
//  		// What about unlabeled arguments?
//  		//
//  		// Notice that "--weirdly-unlabeled" looks like a switch but comes through nicely as unlabeled.
//  		// That's because the example command line indicated "--" before it, which ends switch parsing
//  		// (everything else becomes unlabeled).
//  		//
//  		std::cout << "Unlabeled:\n";
//  		parser.each_unlabeled_argument( []( const std::string& arg ) { std::cout << "\t" << arg << std::endl; } );
//  	}
//  	catch( const std::exception& e )
//  	{
//  		std::cerr << "Error reading argument values: " << e.what() << std::endl;
//  	}
//  	
//  	return 0;
//  }
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

namespace args
{
	// Utility functions and macros.
	//
	template< typename Function >
	std::string collect_string( Function&& fn )
	{
		return fn();
	}
#define args_collect_string( expression )	\
	args::collect_string( [&]() { std::ostringstream stream; stream << expression; return stream.str(); } )
	
#define ARGS_EXCEPTION( exception_class ) \
	struct exception_class : public std::runtime_error { using runtime_error::runtime_error;  };
	
	enum requirement
	{
		optional,
		required
	};
	
	template< typename ValueT >
	struct type_traits
	{
		static constexpr bool always_requires_value() { return false; }
		static constexpr const char* name();
	};
	
	// argument classes.
	//
	class argument_abstract
	{
	public:
		
		ARGS_EXCEPTION( MissingValue )
		ARGS_EXCEPTION( Nameless )
		
		std::string best_name() const
		{
			if( _long_form.empty() )
			{
				return std::string{ _short_form };
			}
			else
			{
				return _long_form;
			}
		}
		
		bool assigned() const
		{
			return _assigned;
		}
		
		const std::string& value_string() const
		{
			return _value;
		}
		
		void clear_value()
		{
			_assigned = false;
			_value.clear();
		}

		// FOR TESTING.
		// Gets the string value of the argument as a result of its conversion to the
		// templated value_t for that particular argument subclass.
		//
		virtual std::string converted_value_string() const = 0;
		virtual bool has_default_value() const = 0;
		
	protected:
		
		bool _assigned;
		
		explicit argument_abstract( const std::string& longForm,
								    const std::string& explanation,
								    char letter,
								    requirement required )
		:	_assigned( false )
		,	_long_form( longForm )
		,	_explanation( explanation )
		,	_short_form( letter )
		,	_requirement( required )
		{
			assert( !_long_form.empty() || _short_form != '\0' );		// Gotta specify at least one.
			assert( _long_form.empty() || _long_form[ 0 ] != '-' );	// Don't start your switch names with -.
			assert( _short_form == '\0' || is_valid_short_form( _short_form ));	// Has to be valid or nothing.
		}
		
		bool has_long_form( const std::string& longForm ) const
		{
			return !_long_form.empty() && _long_form == longForm;
		}
		
		bool has_short_form( char shortForm ) const
		{
			return _short_form != '\0' && _short_form == shortForm;
		}

		bool required() const
		{
			return _requirement == requirement::required;
		}
		
		virtual bool required_value() const = 0;

		void assign( const std::string& givenKey, std::string&& valueString )
		{
			_assigned = true;
			
			_value = std::move( valueString );
			
			if( _value.empty() && required_value() )
			{
				throw MissingValue{ args_collect_string( "Argument required a value but received none." ) };
			}
		}
								
		void print( std::ostream& out ) const
		{
			out << "    ";
			
			bool hasLetter = _short_form != '\0';
			if( hasLetter )
			{
				out << "-" << _short_form;
			}
			
			bool hasLong = !_long_form.empty();
			
			if( hasLetter && hasLong )
			{
				out << ", ";
			}
			
			if( hasLong )
			{
				out << "--" << _long_form;
			}
			
			if( required_value() )
			{
				out << "=<" << value_type_name() << ">";
			}
			
			out << "\n\t\t";
			
			if( _requirement == requirement::required )
			{
				out << "[required] ";
			}
			
			out << _explanation;
		}

		static bool is_valid_short_form( char c )
		{
			return std::isalpha( c );
		}
		
		virtual std::string value_type_name() const = 0;
		
	private:
		
		std::string _long_form;
		std::string _explanation;
		char _short_form;
		requirement _requirement;
		
		std::string _value;
		
		friend class parser;
	};
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	template< typename ValueType >
	class argument : public argument_abstract
	{
	public:
		typedef ValueType value_t;
		
		// Exception types.
		//
		ARGS_EXCEPTION( ValueConversionError )

		operator value_t() const
		{
			value_t result = _default_value;
			
			if( _assigned )
			{
				std::istringstream stream( value_string() );
				stream >> std::boolalpha >> result;
				
				if( stream.fail() )
				{
					throw ValueConversionError{
						args_collect_string( "Could not convert value '" << value_string()
						<< "' to the desired argument type." )};
				}
			}
			
			return result;
		}
		
		value_t value() const { return operator value_t(); }
		
	private:
		
		value_t _default_value = value_t{};

		explicit argument( const std::string& longForm,
						   const std::string& explanation,
						   char letter,
						   requirement required,
						   const value_t& defaultValue )
		:	argument_abstract( longForm, explanation, letter, required )
		,	_default_value( defaultValue )
		{}
		
		virtual std::string value_type_name() const override
		{
			return type_traits< value_t >::name();
		}
		
		virtual std::string converted_value_string() const override
		{
			std::ostringstream stream;
			stream << std::boolalpha << value();
			return stream.str();
		}
		
		virtual bool has_default_value() const override
		{
			return value() == _default_value;
		}
		
		virtual bool required_value() const override
		{
			return type_traits< value_t >::always_requires_value();
		}

		friend class parser;
	};
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
	class parser
	{
	public:
		
		// Exception classes.
		//
		ARGS_EXCEPTION( InvalidParameters )
		ARGS_EXCEPTION( UnknownArgument )
		ARGS_EXCEPTION( FoundDoubleHyphensLeadingNowhere )
		ARGS_EXCEPTION( InvalidArgumentCharacter )
		ARGS_EXCEPTION( MissingRequiredArguments )
		
		explicit parser( const std::string& programName, const std::string& programDescription )
		:	_program( programName )
		,	_description( programDescription )
		{}

		template< typename ValueT >
		argument< ValueT >& add( const std::string& longForm,
								 const std::string& explanation,
								 char letter = '\0',
								 requirement required = requirement::optional,
								 const ValueT& defaultValue = ValueT{} )
		{
			// Verify that no other argument has this longForm or letter.
			//
			assert( !has_long_form_argument( longForm ));
			assert( !has_letter_argument( letter ));
			
			argument< ValueT >* arg = new argument< ValueT >{
				longForm,
				explanation,
				letter,
				required,
				defaultValue };
			
			_args.emplace_back( arg );
			
			return *arg;
		}
		
		bool has_long_form_argument( const std::string& longForm ) const
		{
			return std::any_of( _args.begin(), _args.end(),
						[&]( const std::unique_ptr< argument_abstract >& arg )
						{
							assert( arg );
							return arg->has_long_form( longForm );
						} );
		}
		
		bool has_letter_argument( char letter ) const
		{
			return std::any_of( _args.begin(), _args.end(),
							   [&]( const std::unique_ptr< argument_abstract >& arg )
							   {
								   assert( arg );
								   return arg->has_short_form( letter );
							   } );
		}
		
		void parse( const int argc, const char* argv[] )
		{
			if( argc == 0 || !argv )
			{
				throw InvalidParameters{ "Received no arguments." };
			}

			// If there's no help argument, add one.
			//
			if( !has_long_form_argument( "help" ))
			{
				add< bool >( "help",
							 "Prints this help message.",
							 !has_letter_argument( 'h' ) ? 'h' : '\0' );
			}
			
			auto iterHelpArg = std::find_if( _args.begin(), _args.end(), []( const std::unique_ptr< argument_abstract >& arg )
										 {
											 return arg->has_long_form( "help" );
										 } );
			assert( _args.end() != iterHelpArg );
			
			const argument< bool >& helpArg = *static_cast< const argument< bool >* >( iterHelpArg->get() );
						
			bool doneWithSwitches = false;		// When false, still looking for switches. When true, all arguments
												// are considered "unlabeled."
			
			for( int i = 1; i < argc; ++i )
			{
				assert( argv[ i ] );
				
				const std::string arg{ argv[ i ] };
				assert( !arg.empty() );
				
				// Determine which configured argument corresponds to this program argument, if any,
				// and parse any value it might have.
				
				// Do we have a leading hyphen?
				//
				if( !doneWithSwitches && arg[ 0 ] == '-' )
				{
					// Yes we do.
					
					// Do we have two?
					//
					if( arg.size() > 1 && arg[ 1 ] == '-' )
					{
						// Yes. So we're expecting a long-form argument.
						
						// Read to the end or to the = sign.
						//
						const auto keyEnd = arg.find_first_of( '=' );
						
						const auto key = arg.substr( 2, keyEnd - 2 );
						
						std::string value;
						
						if( keyEnd < arg.size() )
						{
							value = arg.substr( keyEnd + 1 );
						}
						
						if( key.empty() )
						{
							// This is either "--" or, more troublingly, "--=..."
							
							// Either way, don't look for arguments any more:
							// anything else is "unlabeled."
							//
							doneWithSwitches = true;
							continue;
						}
						
						// Find this argument.
						//
						auto& argument = find_matching_argument( key, true /* long form */ );
						process_argument( argument, arg, std::move( value ));
					}
					else
					{
						// No we don't. Just one.
						
						// Consider each following letter to be a short-form argument letter.
						//
						for( size_t i = 1; i < arg.size(); ++i )
						{
							const char c = arg[ i ];
							
							// Is this a reasonable argument character?
							//
							if( argument_abstract::is_valid_short_form( c ))
							{
								// This is a legitimate argument.
								
								// Does it have a value?
								//
								std::string value;
								bool hasAssignment = i + 1 < arg.size() && arg[ i + 1 ] == '=';
								if( hasAssignment )
								{
									// Looks like it. Read it.
									//
									value = arg.substr( i + 2 );
								}
								auto& argument = find_matching_argument( std::string{ c }, false /* short form */ );
								process_argument( argument, arg, std::move( value ));
								
								if( hasAssignment )
								{
									break;
								}
							}
							else
							{
								throw InvalidArgumentCharacter{
									args_collect_string( "Invalid argument with character '" << c << "'." ) };
							}
						}
					}
				}
				else
				{
					// No we don't. This is an unlabeled argument.
					//
					_unlabeled_args.emplace_back( std::move( arg ));
				}
			}
			
			// Did each of the *required* arguments get assigned?
			//
			if( std::any_of( _args.begin(), _args.end(),
							[&]( const std::unique_ptr< argument_abstract >& arg )
							{
								assert( arg );
								return !arg->assigned() && arg->required();
							} ))
			{
				throw MissingRequiredArguments{ "Some required arguments were missing." };
			}
			
			// Did our help argument get set?
			//
			if( helpArg.value() )
			{
				show_usage();
			}
		}
		
		void show_usage( std::ostream& out = std::cout ) const
		{
			out << _program << ": " << _description << std::endl;
			out << "usage: " << _program << std::endl;
			
			for( const auto& arg : _args )
			{
				assert( arg );
				arg->print( out );
				out << std::endl;
			}
			out << std::endl;
		}
		
		template< typename Function >
		void each_argument( Function&& fn ) const
		{
			std::for_each( _args.begin(), _args.end(),
						  [&]( const std::unique_ptr< argument_abstract >& arg )
						  {
							  assert( arg );
							  fn( *arg );
						  } );
		}
		

		template< typename Function >
		void each_unlabeled_argument( Function&& fn ) const
		{
			std::for_each( _unlabeled_args.begin(), _unlabeled_args.end(), fn );
		}
		
		void clear_values()
		{
			std::for_each( _args.begin(), _args.end(),
						  []( const std::unique_ptr< argument_abstract >& arg )
						  {
							  assert( arg );
							  arg->clear_value();
						  } );
			
			_unlabeled_args.clear();
		}
		
	protected:
		
		argument_abstract& find_matching_argument( const std::string& argString, bool longForm )
		{
			auto iterFound = std::find_if( _args.begin(), _args.end(),
										  [&]( const std::unique_ptr< argument_abstract >& arg )
										  {
											  assert( arg );
											  if( longForm )
											  {
												  return arg->has_long_form( argString );
											  }
											  else
											  {
												  return arg->has_short_form( argString[ 0 ] );
											  }
											
										  } );
			
			if( iterFound != _args.end() )
			{
				return **iterFound;
			}
			else
			{
				throw UnknownArgument{ args_collect_string( "Unrecognized argument \"-" << ( longForm ? "-" : "" )
															 << argString << "\"." ) };
			}
		}
		
		void process_argument( argument_abstract& argument, const std::string& key, std::string&& value )
		{
			argument.assign( key, std::move( value ));
		}
		
	private:

		std::string _program;
		std::string _description;
		std::vector< std::unique_ptr< argument_abstract >> _args;
		std::vector< std::string > _unlabeled_args;
	};
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Specializations
	//
    template<>
	inline argument< std::string >::operator std::string() const
	{
		return _assigned ? value_string() : _default_value;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// args::type_traits
	//
	template<>
	struct type_traits< bool >
	{
		static constexpr bool always_requires_value() { return false; }
		static constexpr const char* name() { return "bool"; }
	};

	template<>
	struct type_traits< std::string >
	{
		static constexpr bool always_requires_value() { return true; }
		static constexpr const char* name() { return "string"; }
	};
	
	template<>
	struct type_traits< float >
	{
		static constexpr bool always_requires_value() { return true; }
		static constexpr const char* name() { return "number"; }
	};
	
	template<>
	struct type_traits< double >
	{
		static constexpr bool always_requires_value() { return true; }
		static constexpr const char* name() { return "number"; }
	};
	
	template<>
	struct type_traits< int >
	{
		static constexpr bool always_requires_value() { return true; }
		static constexpr const char* name() { return "int"; }
	};

	template<>
	struct type_traits< size_t >
	{
		static constexpr bool always_requires_value() { return true; }
		static constexpr const char* name() { return "size_t"; }
	};
	
#undef args_collect_string
#undef ARGS_EXCEPTION

}

#endif
