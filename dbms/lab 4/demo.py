import sqlite3
from app import connect, PH

class Args:
    backend = 'sqlite'
    db = 'library.db'

conn = connect(Args())
cur = conn.cursor()

# Get initial copies of Book 1
before = conn.execute('SELECT copies FROM books WHERE book_id=1').fetchone()[0]
print(f'Before copies: {before}')

try:
    conn.execute('BEGIN')
    # 1. Update (decrement copies)
    cur.execute('UPDATE books SET copies = copies - 1 WHERE book_id = 1')
    
    # 2. DELIBERATE BUG
    raise RuntimeError('boom')
    
    # 3. Insert (this never happens)
    cur.execute(f'INSERT INTO loans (book_id, member_id) VALUES (1, 1)')
    conn.commit()
except Exception as e:
    conn.rollback()
    print(f'  borrow failed (rolled back): {e}')

# Get copies after rollback
after = conn.execute('SELECT copies FROM books WHERE book_id=1').fetchone()[0]
print(f'After copies: {after}')

conn.close()
