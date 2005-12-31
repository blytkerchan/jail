#ifndef _compiler_tokenizer_h
#define _compiler_tokenizer_h

namespace Jail
{
	namespace Compiler
	{
		namespace Detail
		{
			class TokenizerData;
		}
		class Token;
		
		class Tokenizer
		{
		public :
			Tokenizer();
			~Tokenizer();
			Tokenizer(const Tokenizer & tokenizer);
			Tokenizer & operator=(Tokenizer tokenizer);
			
			Token * next();
			
			void AddBuffer(const char * buf, uint32_t len);
			void AddBuffer(const std::string & buf);
			void AddBuffer(FILE * file);
			void AddBuffer(std::istream & buf);
			void AddBuffer(const std::vector<char> & buf);

		private :
			TokenizerData * data_;
		};
	}
}

#endif

