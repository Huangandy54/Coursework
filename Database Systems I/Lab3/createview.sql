--Lab3 createview.sql
--ahuang44

CREATE VIEW costView AS 
	SELECT b.visitID m.price*b.quantity AS calculatedCost
	FROM billEntry AS b, 
	LEFT JOIN menuItem AS m
		ON b.menuItemID = m.menuItemID