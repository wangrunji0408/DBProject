# SQL Specification

## Tokens

- Sequence matched by regex `'([^']||'')*'` is a string token
- Sequence start woth `--` and end with a line-feed or EOS should be ignored
- Sequence start woth `/*` and end with `*/` or EOS should be ignored
- Sequence matched by regex `[a-zA-Z_][0-9a-zA-Z_]*` is a keyword or identifier token
	- It is a keyword token when it is one of these(case insensitively):
		- `AND` `CREATE` `DATABASE` `DATABASES` `DATE` `DELETE` `DESC` `DROP` `FLOAT` `FOREIGN` `FROM`
		- `INDEX` `INSERT` `INT` `INTO` `IS` `KEY` `NOT` `NULL`
		- `PRIMARY` `REFERENCES` `SELECT` `SET` `SHOW` `TABLE` `TABLES`
		- `UNIQUE` `UPDATE` `USE` `VALUES` `VARCHAR` `WHERE`
	- In any other case it is a identifier token
- Sequence matched by regex `[0-9]*([0-9]\.|\.[0-9])[0-9]*` is a float token
- Sequence matched by regex `[0-9]*` is a int token
- Sequence `;` `,` `=` `<>` `>=` `<=` `>` `<` is also token

Note: in interactive enviroment, `\` followed by a line-feed is replaced with a single line-feed.

## Grammars

- Program:
	- Statement? (';' Statement?)*
- Statement:
	- SHOW DATABASES
	- CREATE DATABASE identifier
	- DROP DATABASE identifier
	- USE identifier | USE DATABASE identifier
	- SHOW TABLES
	- SHOW TABLE identifier
	- DESC identifier
	- CREATE TABLE identifier '(' TableDefine ')'
	- DROP TABLE identifier
	- CREATE INDEX identifier '(' identifier ')'
	- DROP INDEX identifier '(' identifier ')'
	- QueryCommand
- TableDefine
	- Field (',' Field)*
- Field
	- identifier Type ColumnConstraint*
	- PRIMARY KEY '(' identifier (',' identifier)*  ')'
	- FOREIGN KEY '(' identifier ')' REFERENCES identifier '(' identifier ')'
- Type
	- INT '(' int ')'
	- FLOAT
	- DATE
	- VARCHAR '(' int ')'
- ColumnConstraint
	- NOT NULL
	- UNIQUE
- QueryCommand
	- INSERT INTO identifier VALUES ValueLists
	- DELETE FROM identifier WHERE Where
	- UPDATE identifier SET Set WHERE Where
	- SELECT Select FROM TableList WHERE where
- Where
	- Condition (AND Condition)*
- Condition
	- identifier Op Value
	- identifier Op identifier
	- identifier IS NOT? NULL
- Set
	- Assign (',' Assign)*
- Assign
	- identifier '=' Value
- Select
	- '*'
	- identifier (',' identifier)
- TableList
	- identifier (',' identifier)
- ValueLists
	- ValueList (',' ValueList)*
- ValueList
	- '(' Value (',' Value)* ')'
- Value
	- NULL
	- int
	- float
	- string
- Op
	- '='
	- '>'
	- '<'
	- '>='
	- '<='
	- '<>'