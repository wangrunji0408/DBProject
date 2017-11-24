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
		- `UPDATE` `USE` `VALUES` `VARCHAR` `WHERE`
	- In any other case it is a identifier token
- Sequence matched by regex `[0-9]*([0-9]\.|\.[0-9])[0-9]*` is a float token
- Sequence matched by regex `[0-9]*` is a int token
- Sequence `;` `,` `=` `<>` `>=` `<=` `>` `<` is also token

Note: in interactive enviroment, `\` followed by a line-feed is ignored

## Grammars

- TODO