--Lab2 QUERY 5

SELECT v.visitID, c.name AS custName, s.name AS serverName
FROM visit AS v
	LEFT JOIN dinnerTable as d
		ON v.dinnerTableID=d.dinnerTableID
	LEFT JOIN customer AS c
		ON v.custID=c.custID
	LEFT JOIN server as s
		ON v.serverID=s.serverID

WHERE v.cost>=200 
AND s.level IS NOT NULL
AND v.numPeople = d.numSeats