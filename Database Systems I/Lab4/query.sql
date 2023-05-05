SELECT custID, status
FROM customer 
WHERE status IN ('H','M','L')
ORDER BY 
	CASE status
		WHEN 'H' THEN 1
		WHEN 'M' THEN 2
		WHEN 'L' THEN 3
		ELSE 5
	END, joinDate ASC;