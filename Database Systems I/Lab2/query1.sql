--Lab2 QUERY 1
--Find the name	and	address	for	all	customers whose	status is ‘H’ (uppercase H)	and	whose name ends
--with the letter ‘n’ (lowercase n).No duplicates should appear in your result.
--

SELECT name, address
FROM customer
WHERE customer.status='H' 
	AND customer.name LIKE '%n'