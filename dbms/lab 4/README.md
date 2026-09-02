# Lab 4

## Part 2
**2.1**
- Which two writes must happen together? `UPDATE books` and `INSERT INTO loans`.
- What inconsistency occurs if only one happens? Book copies decrease without a loan record, or a loan is created without decreasing copies.
- Where exactly does the rollback occur when copies are 0? At the line `if row[0] <= 0: raise ValueError("no copies available")`, which triggers the except block and `conn.rollback()`.

**2.3**
- Before copies: 1
- After copies: 1

## Part 3
**3.2**
The input `%' OR '1'='1' --` is directly concatenated into the SQL string. It closes the `LIKE` clause string, adds an `OR` condition that always evaluates to true, and comments out the rest of the query (`--`), returning all rows.

**3.4 Rule**
"Never build SQL by string concatenation; always pass user input through placeholders."

## Part 4
**4.3**
- Changed `--backend postgres` to load `psycopg`.
- Modified connection string in `connect()` to use local PostgreSQL credentials.
- Replaced `search_books_unsafe` with `search_books` in option 6.
