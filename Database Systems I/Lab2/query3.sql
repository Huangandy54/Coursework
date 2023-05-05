--Lab2 QUERY 3
--Find the custID and name for each	customer who had at	least two different	visits that	were between
--December 18, 2018	at 8am and December	21, 2018 at 11pm. No duplicates	should appear in your result.
--


SELECT v.custID, c.name
FROM visit AS v
	LEFT JOIN customer AS c
		ON v.custID=c.custID
WHERE v.custArrive between '2018-12-18 08:00:00' and '2018-12-21 23:00:00' 
AND v.custDepart between '2018-12-18 08:00:00' and '2018-12-21 23:00:00'
GROUP BY v.custID, c.name
HAVING COUNT(v.custID)>1