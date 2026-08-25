-- 1. Number of products and average price per category
SELECT c.cat_name, COUNT(p.prod_id) AS num_products, AVG(p.price) AS avg_price
FROM categories c
JOIN products p ON c.cat_id = p.cat_id
GROUP BY c.cat_id, c.cat_name;

-- 2. Total revenue per category, highest first
SELECT c.cat_name, SUM(oi.quantity * oi.unit_price) AS total_revenue
FROM categories c
JOIN products p ON c.cat_id = p.cat_id
JOIN order_items oi ON p.prod_id = oi.prod_id
GROUP BY c.cat_id, c.cat_name
ORDER BY total_revenue DESC;

-- 3. Total revenue per customer country
SELECT cu.country, SUM(oi.quantity * oi.unit_price) AS total_revenue
FROM customers cu
JOIN orders o ON cu.cust_id = o.cust_id
JOIN order_items oi ON o.order_id = oi.order_id
GROUP BY cu.country
ORDER BY total_revenue DESC;

-- 4. Customers who placed more than 3 orders
SELECT cu.cust_id, cu.name, COUNT(o.order_id) AS num_orders
FROM customers cu
JOIN orders o ON cu.cust_id = o.cust_id
GROUP BY cu.cust_id, cu.name
HAVING COUNT(o.order_id) > 3;

-- 5. For each product, total units sold — including products never sold
SELECT p.prod_id, p.prod_name, COALESCE(SUM(oi.quantity), 0) AS units_sold
FROM products p
LEFT JOIN order_items oi ON p.prod_id = oi.prod_id
GROUP BY p.prod_id, p.prod_name;

-- 6. Months whose total revenue exceeded 15,000
SELECT strftime('%Y-%m', o.order_date) AS month, SUM(oi.quantity * oi.unit_price) AS revenue
FROM orders o
JOIN order_items oi ON o.order_id = oi.order_id
GROUP BY month
HAVING SUM(oi.quantity * oi.unit_price) > 15000;

-- 7. Per category: product count, average price, units sold, and revenue
WITH cat_products AS (
    SELECT cat_id, COUNT(prod_id) AS num_products, AVG(price) AS avg_price
    FROM products
    GROUP BY cat_id
),
cat_sales AS (
    SELECT p.cat_id, SUM(oi.quantity) AS units_sold, SUM(oi.quantity * oi.unit_price) AS revenue
    FROM products p
    JOIN order_items oi ON p.prod_id = oi.prod_id
    GROUP BY p.cat_id
)
SELECT c.cat_name, cp.num_products, cp.avg_price, 
       COALESCE(cs.units_sold, 0) AS units_sold, 
       COALESCE(cs.revenue, 0) AS revenue
FROM categories c
JOIN cat_products cp ON c.cat_id = cp.cat_id
LEFT JOIN cat_sales cs ON c.cat_id = cs.cat_id;

-- 8. Products priced above the overall average product price
SELECT prod_id, prod_name, price
FROM products
WHERE price > (SELECT AVG(price) FROM products);

-- 9. Products priced above their own category's average price
SELECT p.prod_id, p.prod_name, p.cat_id, p.price
FROM products p
WHERE price > (
    SELECT AVG(price) 
    FROM products p2 
    WHERE p2.cat_id = p.cat_id
);

-- 10. Customers who have never placed an order
-- Answer: NOT IN is risky because if the subquery returns any NULL values 
-- (example : if an order had a NULL cust_id), the NOT IN condition evaluates 
-- to unknown rather than true, and will silently return 0 rows for everyone.
SELECT cust_id, name
FROM customers c
WHERE NOT EXISTS (
    SELECT 1 FROM orders o WHERE o.cust_id = c.cust_id
);

-- 11. Customers who ordered in 2023 but not in 2022
SELECT cust_id, name
FROM customers c
WHERE EXISTS (
    SELECT 1 FROM orders o 
    WHERE o.cust_id = c.cust_id AND strftime('%Y', order_date) = '2023'
) 
AND NOT EXISTS (
    SELECT 1 FROM orders o 
    WHERE o.cust_id = c.cust_id AND strftime('%Y', order_date) = '2022'
);

-- 12. The product(s) with the maximum price
SELECT prod_id, prod_name, price
FROM products
WHERE price = (SELECT MAX(price) FROM products);

-- 13. Customers who have bought every product in the 'Books' category
-- Answer: Mia Schmidt (18) qualifies as she has bought all 4 books. 
SELECT cust_id, name
FROM customers c
WHERE NOT EXISTS (
    SELECT 1 FROM products p
    JOIN categories cat ON p.cat_id = cat.cat_id
    WHERE cat.cat_name = 'Books'
    AND NOT EXISTS (
        SELECT 1 FROM orders o
        JOIN order_items oi ON o.order_id = oi.order_id
        WHERE o.cust_id = c.cust_id AND oi.prod_id = p.prod_id
    )
);

-- 14. Top-spending customer per country
WITH cust_revenue AS (
    SELECT o.cust_id, SUM(oi.quantity * oi.unit_price) AS revenue
    FROM orders o
    JOIN order_items oi ON o.order_id = oi.order_id
    GROUP BY o.cust_id
),
ranked_customers AS (
    SELECT c.country, c.cust_id, c.name, cr.revenue,
           RANK() OVER (PARTITION BY c.country ORDER BY cr.revenue DESC) AS rank
    FROM customers c
    JOIN cust_revenue cr ON c.cust_id = cr.cust_id
)
SELECT country, cust_id, name, revenue
FROM ranked_customers
WHERE rank = 1;

-- 15. All transitive prerequisites of 'CS-347'
WITH RECURSIVE prereq_chain AS (
    SELECT prereq_id AS course_id
    FROM prereq
    WHERE course_id = 'CS-347'
    UNION
    SELECT p.prereq_id
    FROM prereq p
    JOIN prereq_chain pc ON p.course_id = pc.course_id
)
SELECT course_id FROM prereq_chain;

-- Stretch goal: Q10 as a LEFT JOIN
SELECT c.cust_id, c.name
FROM customers c
LEFT JOIN orders o ON c.cust_id = o.cust_id
WHERE o.order_id IS NULL;
