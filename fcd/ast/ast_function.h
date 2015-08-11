//
// ast_function.h
// Copyright (C) 2015 Félix Cloutier.
// All Rights Reserved.
//
// This file is part of fcd.
// 
// fcd is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// fcd is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with fcd.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef ast_function_cpp
#define ast_function_cpp

#include "ast_nodes.h"
#include "dumb_allocator.h"
#include "llvm_warnings.h"

SILENCE_LLVM_WARNINGS_BEGIN()
#include <llvm/IR/Function.h>
#include <llvm/Support/raw_ostream.h>
SILENCE_LLVM_WARNINGS_END()

#include <list>
#include <unordered_map>
#include <unordered_set>


// The FunctionNode's lifetime is tied to the lifetime of its memory pool (because the lifetime of almost everything it
// contains is), but it is not itself intended to be allocated through the DumbAllocator interface. FunctionNode needs
// more complex data structures that I have no intention of replicating à la PooledDeque, and thus has a non-trivial
// destructor.
class FunctionNode
{
	llvm::Function& function;
	std::list<DeclarationNode*> declarations;
	std::unordered_map<llvm::Value*, Expression*> valueMap;
	
	std::string createName(const std::string& prefix) const;
	Expression* createDeclaration(llvm::Type& type);
	Expression* createDeclaration(llvm::Type& type, const std::string& name);
	void assign(Expression* left, Expression* right);
	Expression* lvalueFor(llvm::Value& value);
	Statement* statementFor(llvm::Instruction& inst);
	
public:
	typedef decltype(declarations)::iterator declaration_iterator;
	
	DumbAllocator pool;
	Statement* body;
	
	static void printIntegerConstant(llvm::raw_ostream&& os, uint64_t constant);
	static void printIntegerConstant(llvm::raw_ostream& os, uint64_t constant);
	static void printPrototype(llvm::raw_ostream& os, llvm::Function& function);
	
	// HACKHACK: I'm not so comfortable receiving a parameter to help disambiguate the stack poiner
	// and figure out locals.
	inline FunctionNode(llvm::Function& fn, llvm::Argument& stackPointer)
	: function(fn), body(nullptr)
	{
	}
	
	inline declaration_iterator decls_begin() { return declarations.begin(); }
	inline declaration_iterator decls_end() { return declarations.end(); }
	inline void erase(declaration_iterator iter) { declarations.erase(iter); }
	
	SequenceNode* basicBlockToStatement(llvm::BasicBlock& bb);
	Expression* valueFor(llvm::Value& value);
	
	void print(llvm::raw_ostream& os) const;
	void dump() const;
};

#endif /* ast_function_cpp */