#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <memory>
#include <functional>

#include "tokenizer.hpp"

int main()
{
    std::cout << "SimplePaser Version 0.1" << std::endl;

    std::unique_ptr<FILE, std::function<void(FILE *)>> inputOwner(
        fopen("data/test.myx", "rt"), 
        [](FILE *file) { (void)fclose(file); 
    });

    if(!inputOwner.get())
    {
        std::cerr << "Unable to open input file." << std::endl;
    }

    fseek(inputOwner.get(), 0, SEEK_END);
    size_t fileSize = ftell(inputOwner.get());
    fseek(inputOwner.get(), 0, SEEK_SET);

    std::string fileContents(fileSize, ' ');
    fread((void*)(fileContents.data()), 1, fileSize, inputOwner.get());

    std::cout << "File contents: \n\n" << fileContents << std::endl;

    SimplePaser::Tokenizer tokenizer;
    std::vector<SimplePaser::Token> tokens = tokenizer.parse(fileContents);

    for (SimplePaser::Token currToken : tokens)
    {
        currToken.debugPrint();
    }

    // std::cout << "Length: " << tokens.size() << std::endl;
}