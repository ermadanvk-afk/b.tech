-- Schema for Part 0 (Mocking Lab 3 tables)
DROP TABLE IF EXISTS products;
CREATE TABLE products (
    prod_id INTEGER PRIMARY KEY,
    prod_name TEXT,
    cat_id INTEGER,
    price REAL
);

DROP TABLE IF EXISTS order_items;
CREATE TABLE order_items (
    order_id INTEGER,
    prod_id INTEGER,
    quantity INTEGER,
    unit_price REAL
);

DROP TABLE IF EXISTS orders;
CREATE TABLE orders (
    order_id INTEGER PRIMARY KEY,
    customer_id INTEGER,
    order_date TEXT,
    total_amount REAL
);

-- 15. Rank products within each category by price, top 2
WITH RankedProducts AS (
    SELECT p.*, RANK() OVER (PARTITION BY cat_id ORDER BY price DESC) as rank
    FROM products p
)
SELECT * FROM RankedProducts WHERE rank <= 2;

-- 16. Running total of order counts by month
WITH MonthlyCounts AS (
    SELECT strftime('%Y-%m', order_date) as month, COUNT(*) as count
    FROM orders
    GROUP BY month
)
SELECT month, count, SUM(count) OVER (ORDER BY month) as running_total
FROM MonthlyCounts;

-- 17. Total spend, rank by spend, spend as percentage of top
WITH CustomerSpends AS (
    SELECT customer_id, SUM(total_amount) as total
    FROM orders
    GROUP BY customer_id
)
SELECT customer_id, total,
       RANK() OVER (ORDER BY total DESC) as rank_by_spend,
       (total * 100.0 / MAX(total) OVER ()) as pct_of_top
FROM CustomerSpends;

-- 19. Create view product_sales
CREATE VIEW product_sales AS
SELECT p.prod_id, p.prod_name,
COALESCE(SUM(oi.quantity), 0) AS units,
COALESCE(SUM(oi.quantity * oi.unit_price), 0) AS revenue
FROM products p
LEFT JOIN order_items oi ON oi.prod_id = p.prod_id
GROUP BY p.prod_id, p.prod_name;

-- 20. 5 best selling products by revenue
SELECT * FROM product_sales ORDER BY revenue DESC LIMIT 5;

-- 21. Products with units = 0
SELECT * FROM product_sales WHERE units = 0;
