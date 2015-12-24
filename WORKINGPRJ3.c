#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"

#define ALLOC(t) (t*) calloc(1, sizeof(t))

// written by me

struct varNode * m, * buffer[200];
int gCount = 0;

struct statementNode* parse_body();

struct varNode * addVarNode(int value, char * name) {

	struct varNode * v = ALLOC(struct varNode);

	v->name = name;
	v->value = value;

	buffer[gCount] = v;
	gCount++;

	return v;
}

struct varNode * search(char * key) {
	for (int i = gCount - 1; i >= 0; i--)
		if (buffer[i]->name != NULL)
			if(strncmp(key,buffer[i]->name,100) == 0)
				return buffer[i];

	return NULL;
}

struct printStatement * parse_print_stmt() {
	struct printStatement * p = ALLOC(struct printStatement);
	char * toPrint;

	while(ttype != SEMICOLON)
		getToken();

	toPrint = strdup(token);
	if (search(toPrint) != NULL)
		  p->id = search(toPrint);

	return p;
}


struct assignmentStatement * parse_assign_stmt()
{
	struct assignmentStatement * a = ALLOC(struct assignmentStatement);

	char * left, * first_id, * leftExp, * rightExp;
	int equaled = 0, count = 0, type = 0, expr = 0, operation, what_it_is;

	while(ttype != SEMICOLON) {

		if (equaled == 1) {
	
			type = ttype;
			first_id = strdup(token);
			
			if (count == 3) {
					// this is an expression
					ungetToken();
					leftExp = strdup(token);

					getToken();
					operation = ttype;

					getToken();
					rightExp = strdup(token);
					expr = 1;
				}
			}

		if (ttype == EQUAL) {
			left = strdup(token);
			equaled = 1;
		}

		getToken();
		count++;
	}
	if (expr == 1) {
		a->op = operation;
		int left_1 = atoi(leftExp);
		int expr_count = atoi(rightExp);

		if (left_1 == 0 && leftExp[0] != '0') {
			// id
			if (search(leftExp) != NULL)
				a->op1 = search(leftExp);
			else
				a->op1 = addVarNode(0, leftExp);
		}
		else
			a->op1 = addVarNode(atoi(leftExp), NULL);
		
		if (expr_count == 0 && rightExp[0] != '0') {
			// id
			if (search(rightExp) != NULL)
				a->op2 = search(rightExp);
			else
				a->op2 = addVarNode(0, rightExp);

		}
		else
			a->op2 = addVarNode(atoi(rightExp), NULL);

		if (search(left) != NULL)
			a->lhs = search(left);
		else
			a->lhs = addVarNode(0, left); // thaddeuskeptical

	}

	else {
		a->op = 0;

		if (type == NUM)
			a->op1 = addVarNode(atoi(first_id), NULL);
		
		if (type == ID) {
			if (search(first_id) != NULL)
				a->op1 = search(first_id);
			else
				a->op1 = addVarNode(0, first_id);
		}
		if (search(left) != NULL)
			a->lhs = search(left);
		else
			a->lhs = addVarNode(0, left);

	}
	return a;
}
struct statementNode* parse_stmt() {
	struct statementNode* st = ALLOC(struct statementNode);

	if (ttype == ID) {
		st->stmt_type = ASSIGNSTMT;
		st->assign_stmt = parse_assign_stmt();// function()
		// set this up as an assignment stmt
		return st;
	}
	
	if (ttype == PRINT) {
		st->stmt_type = PRINTSTMT;
		st->print_stmt = parse_print_stmt();// function()

		return st;
	}

	if (ttype == IF) {
		struct ifStatement * ifs;
		ifs = ALLOC(struct ifStatement);

		st->stmt_type = IFSTMT;
		st->if_stmt = ifs;

		getToken();

		char * temp_char = strdup(token);
		int temp_int = atoi(temp_char);

		if (temp_int == 0 && temp_char[0] != '0')
			ifs->op1 = search(strdup(token));
		else
			ifs->op1 = addVarNode(atoi(temp_char), NULL);


		getToken();
		ifs->relop = ttype;
		getToken();

		ifs->op2 = search(strdup(token));

		temp_char = strdup(token);
		temp_int = atoi(temp_char);

		if (temp_int == 0 && temp_char[0] != '0')
			ifs->op2 = search(strdup(token));
		else
			ifs->op2 = addVarNode(atoi(temp_char), NULL);


		getToken();
		ifs->true_branch = parse_body();

		struct statementNode * temp_statement = ALLOC(struct statementNode);

		temp_statement->stmt_type = NOOPSTMT;

		// appending no-op to the end of the true
		struct statementNode *temp1 = ifs->true_branch;

		while(temp1->next != NULL)
			temp1 = temp1->next;

		temp1->next = temp_statement;
		ifs->false_branch = temp_statement;
		st->next = temp_statement;

		return st;

	}

	if (ttype == WHILE) {
		struct ifStatement * ifs = ALLOC(struct ifStatement);

		st->stmt_type = IFSTMT;
		st->if_stmt = ifs;

		getToken();

		char * temp_char_while = strdup(token);
		int temp_int = atoi(temp_char_while);

		if (temp_int == 0 && temp_char_while[0] != '0')
			ifs->op1 = search(strdup(token));
		else
			ifs->op1 = addVarNode(atoi(temp_char_while), NULL);


		getToken();

		ifs->relop = ttype;
		getToken();

		char * temp_char_while_2 = strdup(token);
		temp_int = atoi(temp_char_while_2);

		if (temp_int == 0 && temp_char_while_2[0] != '0')
			ifs->op2 = search(strdup(token));
		else
			ifs->op2 = addVarNode(atoi(temp_char_while_2), NULL);
		

		getToken();
		ifs->true_branch = parse_body();

		struct statementNode * gt = ALLOC(struct statementNode);

		gt->stmt_type = GOTOSTMT;

		struct gotoStatement * goto_node = ALLOC(struct gotoStatement);

		gt->goto_stmt = goto_node;

		goto_node->target = st;

		struct statementNode *temp1 = ifs->true_branch;

		while(temp1->next != NULL)
			temp1 = temp1->next;

		temp1->next = gt;

		struct statementNode * temp_statement = ALLOC(struct statementNode);
		temp_statement->stmt_type = NOOPSTMT;
		ifs->false_branch = temp_statement;
		st->next = temp_statement;

		return st;
	}

	if (ttype == SWITCH) {
		char *var = strdup(token);
	}
	return NULL;
}


struct statementNode* parse_stmt_list() {

	struct statementNode * st, * stl;   // statement, statement list

	st = parse_stmt();
	getToken();

	if (ttype == LBRACE) 
		printf("error parsing body in parse_stmt_list()\n");
	

	if (ttype == IF || ttype == ID || ttype == WHILE || ttype == SWITCH || ttype == PRINT) {
		stl = parse_stmt_list();

		if (st->stmt_type == IFSTMT) 
			st->next->next = stl;
		
		else 
			st->next = stl;
		
		return st;
	} 
	else { 
		ungetToken();
		return st; 
	}
}


 struct statementNode* parse_body() {
	struct statementNode* stl = ALLOC(struct statementNode);

	if (ttype == LBRACE) {
		getToken();
		while (ttype != RBRACE) {
			stl = parse_stmt_list();
		}
	}

	if (ttype == RBRACE) {
		getToken();
		return stl; 
	}

	printf("error parsing body\n");
	return NULL;

}

int get_var_list(struct varNode * array[]) {
	// gets the next token until we hit a LBRACE
	int count = 0;

	while (ttype != LBRACE)  {
		if (ttype == ID) {
			struct varNode * m;
			m = ALLOC(struct varNode);

			m->name = strdup(token);
			m->value = 0;
			array[count] = m;
			count++;
		}
		getToken();
	}

	return count;
}

struct statementNode * parse_program_and_generate_intermediate_representation() {

	getToken();

	if (ttype == ID) {
		ungetToken();
		gCount = get_var_list(buffer);
	}

	struct statementNode* stl = ALLOC(struct statementNode);
	stl = parse_body();

	return stl;
}